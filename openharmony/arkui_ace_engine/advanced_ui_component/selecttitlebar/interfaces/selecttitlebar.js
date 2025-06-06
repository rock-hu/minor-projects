/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

if (!('finalizeConstruction' in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, 'finalizeConstruction', () => {
    });
}

const KeyCode = requireNapi('multimodalInput.keyCode').KeyCode;
const hilog = requireNapi('hilog');
const SymbolGlyphModifier = requireNapi('arkui.modifier').SymbolGlyphModifier;

const PUBLIC_BACK = {
    'id': -1,
    'type': 40000,
    params: ['sys.symbol.arrow_left'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__',
};
const PUBLIC_MORE = {
    'id': -1,
    'type': 40000,
    params: ['sys.symbol.dot_grid_2x2'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__',
};
const RESOURCE_TYPE_SYMBOL = 40000;
const TEXT_EDITABLE_DIALOG = '18.3fp';
const IMAGE_SIZE = '64vp';
const MAX_DIALOG = '256vp';
const MIN_DIALOG = '216vp';

class ButtonGestureModifier {
    constructor(controller) {
        this.fontSize = 1;
        this.controller = null;
        this.controller = controller;
    }

    applyGesture(event) {
        if (this.fontSize >= ButtonGestureModifier.minFontSize) {
            event.addGesture(new LongPressGestureHandler({
                repeat: false,
                duration: ButtonGestureModifier.longPressTime
            })
                .onAction(() => {
                    if (event) {
                        this.controller?.open();
                    }
                })
                .onActionEnd(() => {
                    this.controller?.close();
                }));
        } else {
            event.clearGestures();
        }
    }
}

ButtonGestureModifier.longPressTime = 500;
ButtonGestureModifier.minFontSize = 1.75;

export class SelectTitleBar extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.__selected = new SynchedPropertySimpleOneWayPU(params.selected, this, 'selected');
        this.options = [];
        this.menuItems = [];
        this.subtitle = '';
        this.badgeValue = 0;
        this.hidesBackButton = false;
        this.messageDesc = '';
        this.onSelected = () => {
        };
        this.__selectMaxWidth = new ObservedPropertySimplePU(0, this, 'selectMaxWidth');
        this.__fontSize = new ObservedPropertySimplePU(1, this, 'fontSize');
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }

    setInitiallyProvidedValue(params) {
        if (params.selected === undefined) {
            this.__selected.set(0);
        }
        if (params.options !== undefined) {
            this.options = params.options;
        }
        if (params.menuItems !== undefined) {
            this.menuItems = params.menuItems;
        }
        if (params.subtitle !== undefined) {
            this.subtitle = params.subtitle;
        }
        if (params.badgeValue !== undefined) {
            this.badgeValue = params.badgeValue;
        }
        if (params.hidesBackButton !== undefined) {
            this.hidesBackButton = params.hidesBackButton;
        }
        if (params.messageDesc !== undefined) {
            this.messageDesc = params.messageDesc;
        }
        if (params.onSelected !== undefined) {
            this.onSelected = params.onSelected;
        }
        if (params.selectMaxWidth !== undefined) {
            this.selectMaxWidth = params.selectMaxWidth;
        }
        if (params.fontSize !== undefined) {
            this.fontSize = params.fontSize;
        }
    }

    updateStateVars(params) {
        this.__selected.reset(params.selected);
    }

    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__selected.purgeDependencyOnElmtId(rmElmtId);
        this.__selectMaxWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__fontSize.purgeDependencyOnElmtId(rmElmtId);
    }

    aboutToBeDeleted() {
        this.__selected.aboutToBeDeleted();
        this.__selectMaxWidth.aboutToBeDeleted();
        this.__fontSize.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    get selected() {
        return this.__selected.get();
    }

    set selected(newValue) {
        this.__selected.set(newValue);
    }

    get selectMaxWidth() {
        return this.__selectMaxWidth.get();
    }

    set selectMaxWidth(newValue) {
        this.__selectMaxWidth.set(newValue);
    }

    get fontSize() {
        return this.__fontSize.get();
    }

    set fontSize(newValue) {
        this.__fontSize.set(newValue);
    }

    getStringByNameSync(contextName) {
        let uiContext = '';
        try {
            uiContext = getContext()?.resourceManager?.getStringByNameSync(contextName);
        } catch (exception) {
            let code = exception?.code;
            let message = exception?.message;
            hilog.error(0x3900, 'Ace', `Faild to getStringByNameSync,cause, code: ${code}, message: ${message}`);
        }
        return uiContext;
    }

    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Flex.create({
                justifyContent: FlexAlign.SpaceBetween,
                alignItems: ItemAlign.Stretch
            });
            Flex.width('100%');
            Flex.height(SelectTitleBar.totalHeight);
            Flex.backgroundColor({
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_background'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            });
            Flex.onAreaChange((_oldValue, newValue) => {
                let newWidth = Number(newValue.width);
                if (!this.hidesBackButton) {
                    newWidth -= ImageMenuItem.imageHotZoneWidth;
                    newWidth += SelectTitleBar.leftPadding;
                    newWidth -= SelectTitleBar.leftPaddingWithBack;
                }
                if (this.menuItems !== undefined) {
                    let menusLength = this.menuItems.length;
                    if (menusLength >= CollapsibleMenuSection.maxCountOfVisibleItems) {
                        newWidth -= ImageMenuItem.imageHotZoneWidth * CollapsibleMenuSection.maxCountOfVisibleItems;
                    } else if (menusLength > 0) {
                        newWidth -= ImageMenuItem.imageHotZoneWidth * menusLength;
                    }
                }
                if (this.badgeValue) {
                    this.selectMaxWidth = newWidth - SelectTitleBar.badgeSize - SelectTitleBar.leftPadding -
                    SelectTitleBar.rightPadding - SelectTitleBar.badgePadding;
                } else {
                    this.selectMaxWidth = newWidth - SelectTitleBar.leftPadding - SelectTitleBar.rightPadding;
                }
            });
        }, Flex);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.margin({
                left: this.hidesBackButton ? {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.ohos_id_max_padding_start'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__',
                } : {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.ohos_id_default_padding_start'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__',
                }
            });
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (!this.hidesBackButton) {
                this.ifElseBranchUpdateFunction(0, () => {
                    {
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            if (isInitialRender) {
                                let componentCall = new ImageMenuItem(this, {
                                    item: {
                                        value: '',
                                        symbolStyle: new SymbolGlyphModifier(PUBLIC_BACK),
                                        isEnabled: true,
                                        label: this.getStringByNameSync('icon_back'),
                                        action: () => this.getUIContext()?.getRouter()?.back()
                                    }, index: -1
                                }, undefined, elmtId, () => {
                                }, { page: 'library/src/main/ets/components/MainPage.ets', line: 117, col: 11 });
                                ViewPU.create(componentCall);
                                let paramsLambda = () => {
                                    return {
                                        item: {
                                            value: '',
                                            symbolStyle: new SymbolGlyphModifier(PUBLIC_BACK),
                                            isEnabled: true,
                                            label: this.getStringByNameSync('icon_back'),
                                            action: () => this.getUIContext()?.getRouter()?.back()
                                        },
                                        index: -1
                                    };
                                };
                                componentCall.paramsGenerator_ = paramsLambda;
                            } else {
                                this.updateStateVarsOfChildByElmtId(elmtId, {});
                            }
                        }, { name: 'ImageMenuItem' });
                    }
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.justifyContent(FlexAlign.Start);
            Column.alignItems(HorizontalAlign.Start);
            Column.constraintSize({ maxWidth: this.selectMaxWidth });
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.badgeValue) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Badge.create({
                            count: this.badgeValue,
                            position: BadgePosition.Right,
                            style: {
                                badgeColor: {
                                    'id': -1,
                                    'type': 10001,
                                    params: ['sys.color.ohos_id_color_emphasize'],
                                    'bundleName': '__harDefaultBundleName__',
                                    'moduleName': '__harDefaultModuleName__',
                                },
                                borderColor: {
                                    'id': -1,
                                    'type': 10001,
                                    params: ['sys.color.ohos_id_color_emphasize'],
                                    'bundleName': '__harDefaultBundleName__',
                                    'moduleName': '__harDefaultModuleName__',
                                },
                                borderWidth: 0
                            }
                        });
                        Badge.accessibilityGroup(true);
                        Badge.accessibilityLevel('no');
                    }, Badge);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Row.create();
                        Row.justifyContent(FlexAlign.Start);
                        Row.margin({
                            right: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.ohos_id_elements_margin_horizontal_l'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            }
                        });
                    }, Row);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Select.create(this.options);
                        Select.selected(this.selected);
                        Select.value(this.selected >= 0 && this.selected < this.options.length ?
                        this.options[this.selected].value : '');
                        Select.font({
                            size: this.hidesBackButton && !this.subtitle
                                ? {
                                    'id': -1,
                                    'type': 10002,
                                    params: ['sys.float.ohos_id_text_size_headline7'],
                                    'bundleName': '__harDefaultBundleName__',
                                    'moduleName': '__harDefaultModuleName__',
                                } : {
                                    'id': -1,
                                    'type': 10002,
                                    params: ['sys.float.ohos_id_text_size_headline8'],
                                    'bundleName': '__harDefaultBundleName__',
                                    'moduleName': '__harDefaultModuleName__',
                                }
                        });
                        Select.fontColor({
                            'id': -1,
                            'type': 10001,
                            params: ['sys.color.ohos_id_color_titlebar_text'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__',
                        });
                        Select.backgroundColor(Color.Transparent);
                        Select.onSelect(this.onSelected);
                        Select.constraintSize({ maxWidth: this.selectMaxWidth });
                        Select.offset({ x: -4 });
                        Select.accessibilityLevel('yes');
                        Select.accessibilityDescription(this.messageDesc.replace('%d', this.badgeValue.toString()));
                    }, Select);
                    Select.pop();
                    Row.pop();
                    Badge.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Row.create();
                        Row.justifyContent(FlexAlign.Start);
                    }, Row);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Select.create(this.options);
                        Select.selected(this.selected);
                        Select.value(this.selected >= 0 && this.selected < this.options.length ?
                        this.options[this.selected].value : '');
                        Select.font({
                            size: this.hidesBackButton && !this.subtitle
                                ? {
                                    'id': -1,
                                    'type': 10002,
                                    params: ['sys.float.ohos_id_text_size_headline7'],
                                    'bundleName': '__harDefaultBundleName__',
                                    'moduleName': '__harDefaultModuleName__',
                                } : {
                                    'id': -1,
                                    'type': 10002,
                                    params: ['sys.float.ohos_id_text_size_headline8'],
                                    'bundleName': '__harDefaultBundleName__',
                                    'moduleName': '__harDefaultModuleName__',
                                }
                        });
                        Select.fontColor({
                            'id': -1,
                            'type': 10001,
                            params: ['sys.color.ohos_id_color_titlebar_text'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__',
                        });
                        Select.backgroundColor(Color.Transparent);
                        Select.onSelect(this.onSelected);
                        Select.constraintSize({ maxWidth: this.selectMaxWidth });
                        Select.offset({ x: -4 });
                    }, Select);
                    Select.pop();
                    Row.pop();
                });
            }
        }, If);
        If.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.subtitle !== undefined) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Row.create();
                        Row.justifyContent(FlexAlign.Start);
                        Row.margin({ left: SelectTitleBar.subtitleLeftPadding });
                    }, Row);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(this.subtitle);
                        Text.fontSize({
                            'id': -1,
                            'type': 10002,
                            params: ['sys.float.ohos_id_text_size_over_line'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__',
                        });
                        Text.fontColor({
                            'id': -1,
                            'type': 10001,
                            params: ['sys.color.ohos_id_color_titlebar_subtitle_text'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__',
                        });
                        Text.maxLines(1);
                        Text.textOverflow({ overflow: TextOverflow.Ellipsis });
                        Text.constraintSize({ maxWidth: this.selectMaxWidth });
                        Text.offset({ y: -4 });
                    }, Text);
                    Text.pop();
                    Row.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Column.pop();
        Row.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.menuItems !== undefined && this.menuItems.length > 0) {
                this.ifElseBranchUpdateFunction(0, () => {
                    {
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            if (isInitialRender) {
                                let componentCall = new CollapsibleMenuSection(this,
                                    { menuItems: this.menuItems, index: 1 + SelectTitleBar.instanceCount++ }, undefined,
                                    elmtId, () => {
                                    }, { page: 'library/src/main/ets/components/MainPage.ets', line: 197, col: 9 });
                                ViewPU.create(componentCall);
                                let paramsLambda = () => {
                                    return {
                                        menuItems: this.menuItems,
                                        index: 1 + SelectTitleBar.instanceCount++
                                    };
                                };
                                componentCall.paramsGenerator_ = paramsLambda;
                            } else {
                                this.updateStateVarsOfChildByElmtId(elmtId, {});
                            }
                        }, { name: 'CollapsibleMenuSection' });
                    }
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Flex.pop();
    }

    aboutToAppear() {
        try {
            let resourceManager = getContext().resourceManager;
            this.messageDesc =
                resourceManager?.getPluralStringByNameSync('selecttitlebar_accessibility_message_desc_new',
                    this.badgeValue);
        } catch (exception) {
            let code = exception?.code;
            let message = exception?.message;
            hilog.error(0x3900, 'Ace', `Faild to getPluralStringByNameSync,cause, code: ${code}, message: ${message}`);
        }
    }

    rerender() {
        this.updateDirtyElements();
    }
}
SelectTitleBar.badgeSize = 16;
SelectTitleBar.totalHeight = 56;
SelectTitleBar.leftPadding = 24;
SelectTitleBar.leftPaddingWithBack = 12;
SelectTitleBar.rightPadding = 24;
SelectTitleBar.badgePadding = 16;
SelectTitleBar.subtitleLeftPadding = 4;
SelectTitleBar.instanceCount = 0;

class CollapsibleMenuSection extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.menuItems = [];
        this.item = {
            symbolStyle: new SymbolGlyphModifier(PUBLIC_MORE),
            label: {
                'id': -1,
                'type': 10003,
                params: ['sys.string.ohos_toolbar_more'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            },
        };
        this.index = 0;
        this.minFontSize = 1.75;
        this.isFollowingSystemFontScale = false;
        this.maxFontScale = 1;
        this.systemFontScale = 1;
        this.firstFocusableIndex = -1;
        this.__isPopupShown = new ObservedPropertySimplePU(false, this, 'isPopupShown');
        this.__isMoreIconOnFocus = new ObservedPropertySimplePU(false, this, 'isMoreIconOnFocus');
        this.__isMoreIconOnHover = new ObservedPropertySimplePU(false, this, 'isMoreIconOnHover');
        this.__isMoreIconOnClick = new ObservedPropertySimplePU(false, this, 'isMoreIconOnClick');
        this.__fontSize = new SynchedPropertySimpleOneWayPU(params.fontSize, this, 'fontSize');
        this.dialogController = new CustomDialogController({
            builder: () => {
                let jsDialog = new SelectTitleBarDialog(this, {
                    cancel: () => {
                    },
                    confirm: () => {
                    },
                    selectTitleDialog: this.item,
                    selectTitleBarDialog: this.item.label ? this.item.label : '',
                    fontSize: this.fontSize,
                }, undefined, -1, () => {
                }, { page: 'library/src/main/ets/components/MainPage.ets', line: 266, col: 14 });
                jsDialog.setController(this.dialogController);
                ViewPU.create(jsDialog);
                let paramsLambda = () => {
                    return {
                        cancel: () => {
                        },
                        confirm: () => {
                        },
                        selectTitleDialog: this.item,
                        selectTitleBarDialog: this.item.label ? this.item.label : '',
                        fontSize: this.fontSize
                    };
                };
                jsDialog.paramsGenerator_ = paramsLambda;
            },
            maskColor: Color.Transparent,
            isModal: true,
            customStyle: true
        }, this);
        this.__buttonGestureModifier =
            new ObservedPropertyObjectPU(new ButtonGestureModifier(this.dialogController), this,
                'buttonGestureModifier');
        this.setInitiallyProvidedValue(params);
        this.declareWatch('fontSize', this.onFontSizeUpdated);
        this.finalizeConstruction();
    }

    setInitiallyProvidedValue(params) {
        if (params.menuItems !== undefined) {
            this.menuItems = params.menuItems;
        }
        if (params.item !== undefined) {
            this.item = params.item;
        }
        if (params.index !== undefined) {
            this.index = params.index;
        }
        if (params.minFontSize !== undefined) {
            this.minFontSize = params.minFontSize;
        }
        if (params.isFollowingSystemFontScale !== undefined) {
            this.isFollowingSystemFontScale = params.isFollowingSystemFontScale;
        }
        if (params.maxFontScale !== undefined) {
            this.maxFontScale = params.maxFontScale;
        }
        if (params.systemFontScale !== undefined) {
            this.systemFontScale = params.systemFontScale;
        }
        if (params.firstFocusableIndex !== undefined) {
            this.firstFocusableIndex = params.firstFocusableIndex;
        }
        if (params.isPopupShown !== undefined) {
            this.isPopupShown = params.isPopupShown;
        }
        if (params.isMoreIconOnFocus !== undefined) {
            this.isMoreIconOnFocus = params.isMoreIconOnFocus;
        }
        if (params.isMoreIconOnHover !== undefined) {
            this.isMoreIconOnHover = params.isMoreIconOnHover;
        }
        if (params.isMoreIconOnClick !== undefined) {
            this.isMoreIconOnClick = params.isMoreIconOnClick;
        }
        if (params.fontSize === undefined) {
            this.__fontSize.set(1);
        }
        if (params.dialogController !== undefined) {
            this.dialogController = params.dialogController;
        }
        if (params.buttonGestureModifier !== undefined) {
            this.buttonGestureModifier = params.buttonGestureModifier;
        }
    }

    updateStateVars(params) {
        this.__fontSize.reset(params.fontSize);
    }

    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__isPopupShown.purgeDependencyOnElmtId(rmElmtId);
        this.__isMoreIconOnFocus.purgeDependencyOnElmtId(rmElmtId);
        this.__isMoreIconOnHover.purgeDependencyOnElmtId(rmElmtId);
        this.__isMoreIconOnClick.purgeDependencyOnElmtId(rmElmtId);
        this.__fontSize.purgeDependencyOnElmtId(rmElmtId);
        this.__buttonGestureModifier.purgeDependencyOnElmtId(rmElmtId);
    }

    aboutToBeDeleted() {
        this.__isPopupShown.aboutToBeDeleted();
        this.__isMoreIconOnFocus.aboutToBeDeleted();
        this.__isMoreIconOnHover.aboutToBeDeleted();
        this.__isMoreIconOnClick.aboutToBeDeleted();
        this.__fontSize.aboutToBeDeleted();
        this.__buttonGestureModifier.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    get isPopupShown() {
        return this.__isPopupShown.get();
    }

    set isPopupShown(newValue) {
        this.__isPopupShown.set(newValue);
    }

    get isMoreIconOnFocus() {
        return this.__isMoreIconOnFocus.get();
    }

    set isMoreIconOnFocus(newValue) {
        this.__isMoreIconOnFocus.set(newValue);
    }

    get isMoreIconOnHover() {
        return this.__isMoreIconOnHover.get();
    }

    set isMoreIconOnHover(newValue) {
        this.__isMoreIconOnHover.set(newValue);
    }

    get isMoreIconOnClick() {
        return this.__isMoreIconOnClick.get();
    }

    set isMoreIconOnClick(newValue) {
        this.__isMoreIconOnClick.set(newValue);
    }

    get fontSize() {
        return this.__fontSize.get();
    }

    set fontSize(newValue) {
        this.__fontSize.set(newValue);
    }

    get buttonGestureModifier() {
        return this.__buttonGestureModifier.get();
    }

    set buttonGestureModifier(newValue) {
        this.__buttonGestureModifier.set(newValue);
    }

    getMoreIconFgColor() {
        return this.isMoreIconOnClick
            ? {
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_titlebar_icon_pressed'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            } : {
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_titlebar_icon'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            };
    }

    getMoreIconBgColor() {
        if (this.isMoreIconOnClick) {
            return {
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_click_effect'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            };
        } else if (this.isMoreIconOnHover) {
            return {
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_hover'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            };
        } else {
            return Color.Transparent;
        }
    }

    aboutToAppear() {
        try {
            let uiContent = this.getUIContext();
            this.isFollowingSystemFontScale = uiContent.isFollowingSystemFontScale();
            this.maxFontScale = uiContent.getMaxFontScale();
        } catch (exception) {
            let code = exception?.code;
            let message = exception?.message;
            hilog.error(0x3900, 'Ace', `Faild to decideFontScale,cause, code: ${code}, message: ${message}`);
        }
        this.menuItems.forEach((item, index) => {
            if (item.isEnabled && this.firstFocusableIndex === -1 &&
                index > CollapsibleMenuSection.maxCountOfVisibleItems - 2) {
                this.firstFocusableIndex = this.index * 1000 + index + 1;
            }
        });
        this.fontSize = this.decideFontScale();
    }

    decideFontScale() {
        let uiContent = this.getUIContext();
        this.systemFontScale = uiContent.getHostContext()?.config?.fontSizeScale ?? 1;
        if (!this.isFollowingSystemFontScale) {
            return 1;
        }
        return Math.min(this.systemFontScale, this.maxFontScale);
    }

    onFontSizeUpdated() {
        this.buttonGestureModifier.fontSize = this.fontSize;
    }

    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.height('100%');
            Column.margin({
                right: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.ohos_id_default_padding_end'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__',
                }
            });
            Column.justifyContent(FlexAlign.Center);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.menuItems.length <= CollapsibleMenuSection.maxCountOfVisibleItems) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        ForEach.create();
                        const forEachItemGenFunction = (_item, index) => {
                            const item = _item;
                            {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    if (isInitialRender) {
                                        let componentCall = new ImageMenuItem(this,
                                            { item: item, index: this.index * 1000 + index + 1 }, undefined, elmtId,
                                            () => {
                                            }, {
                                                page: 'library/src/main/ets/components/MainPage.ets',
                                                line: 336,
                                                col: 13
                                            });
                                        ViewPU.create(componentCall);
                                        let paramsLambda = () => {
                                            return {
                                                item: item,
                                                index: this.index * 1000 + index + 1
                                            };
                                        };
                                        componentCall.paramsGenerator_ = paramsLambda;
                                    } else {
                                        this.updateStateVarsOfChildByElmtId(elmtId, {});
                                    }
                                }, { name: 'ImageMenuItem' });
                            }
                        };
                        this.forEachUpdateFunction(elmtId, this.menuItems, forEachItemGenFunction, undefined, true,
                            false);
                    }, ForEach);
                    ForEach.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        ForEach.create();
                        const forEachItemGenFunction = (_item, index) => {
                            const item = _item;
                            {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    if (isInitialRender) {
                                        let componentCall = new ImageMenuItem(this,
                                            { item: item, index: this.index * 1000 + index + 1 }, undefined, elmtId,
                                            () => {
                                            }, {
                                                page: 'library/src/main/ets/components/MainPage.ets',
                                                line: 341,
                                                col: 15
                                            });
                                        ViewPU.create(componentCall);
                                        let paramsLambda = () => {
                                            return {
                                                item: item,
                                                index: this.index * 1000 + index + 1
                                            };
                                        };
                                        componentCall.paramsGenerator_ = paramsLambda;
                                    } else {
                                        this.updateStateVarsOfChildByElmtId(elmtId, {});
                                    }
                                }, { name: 'ImageMenuItem' });
                            }
                        };
                        this.forEachUpdateFunction(elmtId,
                            this.menuItems.slice(0, CollapsibleMenuSection.maxCountOfVisibleItems - 1),
                            forEachItemGenFunction, undefined, true, false);
                    }, ForEach);
                    ForEach.pop();
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Button.createWithChild({ type: ButtonType.Normal, stateEffect: true });
                        Button.accessibilityText({
                            'id': -1,
                            'type': 10003,
                            params: ['sys.string.ohos_toolbar_more'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__',
                        });
                        Button.width(ImageMenuItem.imageHotZoneWidth);
                        Button.height(ImageMenuItem.imageHotZoneWidth);
                        Button.borderRadius(ImageMenuItem.buttonBorderRadius);
                        Button.foregroundColor(this.getMoreIconFgColor());
                        Button.backgroundColor(this.getMoreIconBgColor());
                        ViewStackProcessor.visualState('focused');
                        Button.border({
                            radius: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.ohos_id_corner_radius_clicked'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            width: ImageMenuItem.focusBorderWidth,
                            color: {
                                'id': -1,
                                'type': 10001,
                                params: ['sys.color.ohos_id_color_focused_outline'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            style: BorderStyle.Solid
                        });
                        ViewStackProcessor.visualState('normal');
                        Button.border({
                            radius: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.ohos_id_corner_radius_clicked'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            width: 0
                        });
                        ViewStackProcessor.visualState();
                        Button.onFocus(() => this.isMoreIconOnFocus = true);
                        Button.onBlur(() => this.isMoreIconOnFocus = false);
                        Button.onHover((isOn) => this.isMoreIconOnHover = isOn);
                        Button.onKeyEvent((event) => {
                            if (event.keyCode !== KeyCode.KEYCODE_ENTER && event.keyCode !== KeyCode.KEYCODE_SPACE) {
                                return;
                            }
                            if (event.type === KeyType.Down) {
                                this.isMoreIconOnClick = true;
                            }
                            if (event.type === KeyType.Up) {
                                this.isMoreIconOnClick = false;
                            }
                        });
                        Button.onTouch((event) => {
                            if (event.type === TouchType.Down) {
                                this.isMoreIconOnClick = true;
                            }
                            if (event.type === TouchType.Up || event.type === TouchType.Cancel) {
                                this.isMoreIconOnClick = false;
                                if (this.fontSize >= this.minFontSize) {
                                    this.dialogController?.close();
                                }
                            }
                        });
                        Button.onClick(() => this.isPopupShown = true);
                        Button.gestureModifier(ObservedObject.GetRawObject(this.buttonGestureModifier));
                        Button.bindPopup(this.isPopupShown, {
                            builder: { builder: this.popupBuilder.bind(this) },
                            placement: Placement.Bottom,
                            popupColor: Color.White,
                            enableArrow: false,
                            onStateChange: (e) => {
                                this.isPopupShown = e.isVisible;
                                if (!e.isVisible) {
                                    this.isMoreIconOnClick = false;
                                }
                            }
                        });
                    }, Button);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        SymbolGlyph.create(PUBLIC_MORE);
                        SymbolGlyph.fontSize(ImageMenuItem.imageSize);
                        SymbolGlyph.draggable(false);
                        SymbolGlyph.fontColor([{
                            'id': -1,
                            'type': 10001,
                            params: ['sys.color.icon_primary'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__',
                        }]);
                        SymbolGlyph.focusable(true);
                    }, SymbolGlyph);
                    Button.pop();
                });
            }
        }, If);
        If.pop();
        Row.pop();
        Column.pop();
    }

    onPlaceChildren(selfLayoutInfo, children, constraint) {
        children.forEach((child) => {
            child.layout({ x: 0, y: 0 });
        });
        this.fontSize = this.decideFontScale();
    }

    popupBuilder(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.width(ImageMenuItem.imageHotZoneWidth +
                CollapsibleMenuSection.focusPadding * CollapsibleMenuSection.marginsNum);
            Column.margin({ top: CollapsibleMenuSection.focusPadding, bottom: CollapsibleMenuSection.focusPadding });
            Column.onAppear(() => {
                focusControl.requestFocus(ImageMenuItem.focusablePrefix + this.firstFocusableIndex);
            });
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            ForEach.create();
            const forEachItemGenFunction = (_item, index) => {
                const item = _item;
                {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        if (isInitialRender) {
                            let componentCall = new ImageMenuItem(this, {
                                item: item, index: this.index * 1000 +
                                CollapsibleMenuSection.maxCountOfVisibleItems + index, isPopup: false
                            }, undefined, elmtId, () => {
                            }, { page: 'library/src/main/ets/components/MainPage.ets', line: 432, col: 11 });
                            ViewPU.create(componentCall);
                            let paramsLambda = () => {
                                return {
                                    item: item,
                                    index: this.index * 1000 +
                                    CollapsibleMenuSection.maxCountOfVisibleItems + index,
                                    isPopup: false
                                };
                            };
                            componentCall.paramsGenerator_ = paramsLambda;
                        } else {
                            this.updateStateVarsOfChildByElmtId(elmtId, {});
                        }
                    }, { name: 'ImageMenuItem' });
                }
            };
            this.forEachUpdateFunction(elmtId,
                this.menuItems.slice(CollapsibleMenuSection.maxCountOfVisibleItems - 1, this.menuItems.length),
                forEachItemGenFunction, undefined, true, false);
        }, ForEach);
        ForEach.pop();
        Column.pop();
    }

    rerender() {
        this.updateDirtyElements();
    }
}

CollapsibleMenuSection.maxCountOfVisibleItems = 3;
CollapsibleMenuSection.focusPadding = 4;
CollapsibleMenuSection.marginsNum = 2;

class ImageMenuItem extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.item = {};
        this.index = 0;
        this.minFontSize = 1.75;
        this.isFollowingSystemFontScale = false;
        this.maxFontScale = 1;
        this.systemFontScale = 1;
        this.isPopup = true;
        this.__isOnFocus = new ObservedPropertySimplePU(false, this, 'isOnFocus');
        this.__isOnHover = new ObservedPropertySimplePU(false, this, 'isOnHover');
        this.__isOnClick = new ObservedPropertySimplePU(false, this, 'isOnClick');
        this.__fontSize = new SynchedPropertySimpleOneWayPU(params.fontSize, this, 'fontSize');
        this.dialogController = new CustomDialogController({
            builder: () => {
                let jsDialog = new SelectTitleBarDialog(this, {
                    cancel: () => {
                    },
                    confirm: () => {
                    },
                    selectTitleDialog: this.item,
                    selectTitleBarDialog: this.item.label ? this.item.label : this.textDialog(),
                    fontSize: this.fontSize,
                }, undefined, -1, () => {
                }, { page: 'library/src/main/ets/components/MainPage.ets', line: 466, col: 14 });
                jsDialog.setController(this.dialogController);
                ViewPU.create(jsDialog);
                let paramsLambda = () => {
                    return {
                        cancel: () => {
                        },
                        confirm: () => {
                        },
                        selectTitleDialog: this.item,
                        selectTitleBarDialog: this.item.label ? this.item.label : this.textDialog(),
                        fontSize: this.fontSize
                    };
                };
                jsDialog.paramsGenerator_ = paramsLambda;
            },
            maskColor: Color.Transparent,
            isModal: true,
            customStyle: true
        }, this);
        this.__buttonGestureModifier =
            new ObservedPropertyObjectPU(new ButtonGestureModifier(this.dialogController), this,
                'buttonGestureModifier');
        this.setInitiallyProvidedValue(params);
        this.declareWatch('fontSize', this.onFontSizeUpdated);
        this.finalizeConstruction();
    }

    setInitiallyProvidedValue(params) {
        if (params.item !== undefined) {
            this.item = params.item;
        }
        if (params.index !== undefined) {
            this.index = params.index;
        }
        if (params.minFontSize !== undefined) {
            this.minFontSize = params.minFontSize;
        }
        if (params.isFollowingSystemFontScale !== undefined) {
            this.isFollowingSystemFontScale = params.isFollowingSystemFontScale;
        }
        if (params.maxFontScale !== undefined) {
            this.maxFontScale = params.maxFontScale;
        }
        if (params.systemFontScale !== undefined) {
            this.systemFontScale = params.systemFontScale;
        }
        if (params.isPopup !== undefined) {
            this.isPopup = params.isPopup;
        }
        if (params.isOnFocus !== undefined) {
            this.isOnFocus = params.isOnFocus;
        }
        if (params.isOnHover !== undefined) {
            this.isOnHover = params.isOnHover;
        }
        if (params.isOnClick !== undefined) {
            this.isOnClick = params.isOnClick;
        }
        if (params.fontSize === undefined) {
            this.__fontSize.set(1);
        }
        if (params.dialogController !== undefined) {
            this.dialogController = params.dialogController;
        }
        if (params.buttonGestureModifier !== undefined) {
            this.buttonGestureModifier = params.buttonGestureModifier;
        }
    }

    updateStateVars(params) {
        this.__fontSize.reset(params.fontSize);
    }

    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__isOnFocus.purgeDependencyOnElmtId(rmElmtId);
        this.__isOnHover.purgeDependencyOnElmtId(rmElmtId);
        this.__isOnClick.purgeDependencyOnElmtId(rmElmtId);
        this.__fontSize.purgeDependencyOnElmtId(rmElmtId);
        this.__buttonGestureModifier.purgeDependencyOnElmtId(rmElmtId);
    }

    aboutToBeDeleted() {
        this.__isOnFocus.aboutToBeDeleted();
        this.__isOnHover.aboutToBeDeleted();
        this.__isOnClick.aboutToBeDeleted();
        this.__fontSize.aboutToBeDeleted();
        this.__buttonGestureModifier.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    get isOnFocus() {
        return this.__isOnFocus.get();
    }

    set isOnFocus(newValue) {
        this.__isOnFocus.set(newValue);
    }

    get isOnHover() {
        return this.__isOnHover.get();
    }

    set isOnHover(newValue) {
        this.__isOnHover.set(newValue);
    }

    get isOnClick() {
        return this.__isOnClick.get();
    }

    set isOnClick(newValue) {
        this.__isOnClick.set(newValue);
    }

    get fontSize() {
        return this.__fontSize.get();
    }

    set fontSize(newValue) {
        this.__fontSize.set(newValue);
    }

    get buttonGestureModifier() {
        return this.__buttonGestureModifier.get();
    }

    set buttonGestureModifier(newValue) {
        this.__buttonGestureModifier.set(newValue);
    }

    textDialog() {
        if (this.item.value === PUBLIC_MORE) {
            return {
                'id': -1,
                'type': 10003,
                params: ['sys.string.ohos_toolbar_more'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            };
        } else if (this.item.value === PUBLIC_BACK) {
            return {
                'id': -1,
                'type': 10003,
                params: ['sys.string.icon_back'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            };
        } else {
            return this.item.label ? this.item.label : '';
        }
    }

    getFgColor() {
        return this.isOnClick
            ? {
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_titlebar_icon_pressed'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            } : {
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_titlebar_icon'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            };
    }

    getBgColor() {
        if (this.isOnClick) {
            return {
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_click_effect'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            };
        } else if (this.isOnHover) {
            return {
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_hover'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__',
            };
        } else {
            return Color.Transparent;
        }
    }

    aboutToAppear() {
        try {
            let uiContent = this.getUIContext();
            this.isFollowingSystemFontScale = uiContent.isFollowingSystemFontScale();
            this.maxFontScale = uiContent.getMaxFontScale();
        } catch (exception) {
            let code = exception.code;
            let message = exception.message;
            hilog.error(0x3900, 'Ace', `Faild to isFollowingSystemFontScale,cause, code: ${code}, message: ${message}`);
        }
        this.fontSize = this.decideFontScale();
    }

    decideFontScale() {
        let uiContent = this.getUIContext();
        this.systemFontScale = uiContent.getHostContext()?.config?.fontSizeScale ?? 1;
        if (!this.isFollowingSystemFontScale) {
            return 1;
        }
        return Math.min(this.systemFontScale, this.maxFontScale);
    }

    toStringFormat(resource) {
        if (typeof resource === 'string') {
            return resource;
        } else if (typeof resource === 'undefined') {
            return '';
        } else {
            let resourceString = '';
            try {
                resourceString = getContext()?.resourceManager?.getStringSync(resource);
            } catch (err) {
                let code = err?.code;
                let message = err?.message;
                hilog.error(0x3900, 'Ace', `Faild to SelectTitleBar toStringFormat,code: ${code},message:${message}`);
            }
            return resourceString;
        }
    }

    getAccessibilityReadText() {
        if (this.item.value === PUBLIC_BACK) {
            try {
                return getContext()?.resourceManager?.getStringByNameSync('icon_back');
            } catch (err) {
                let code = err?.code;
                let message = err?.message;
                hilog.error(0x3900, 'Ace', `Faild to SelectTitleBar toStringFormat,code: ${code},message:${message}`);
            }
        } else if (this.item.value === PUBLIC_MORE) {
            try {
                return getContext()?.resourceManager?.getStringByNameSync('ohos_toolbar_more');
            } catch (err) {
                let code = err?.code;
                let message = err?.message;
                hilog.error(0x3900, 'Ace', `Faild to SelectTitleBar toStringFormat,code: ${code},message:${message}`);
            }
        } else if (this.item.accessibilityText) {
            return this.item.accessibilityText;
        } else if (this.item.label) {
            return this.item.label;
        }
        return ' ';
    }

    onPlaceChildren(selfLayoutInfo, children, constraint) {
        children.forEach((child) => {
            child.layout({ x: 0, y: 0 });
        });
        this.fontSize = this.decideFontScale();
    }

    onFontSizeUpdated() {
        this.buttonGestureModifier.fontSize = this.fontSize;
    }

    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.isPopup) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Button.createWithChild({ type: ButtonType.Normal, stateEffect: this.item.isEnabled });
                        Button.accessibilityText(this.getAccessibilityReadText());
                        Button.accessibilityLevel(this.item?.accessibilityLevel ?? 'auto');
                        Button.accessibilityDescription(this.item?.accessibilityDescription);
                        Button.width(ImageMenuItem.imageHotZoneWidth);
                        Button.height(ImageMenuItem.imageHotZoneWidth);
                        Button.borderRadius(ImageMenuItem.buttonBorderRadius);
                        Button.foregroundColor(this.getFgColor());
                        Button.backgroundColor(this.getBgColor());
                        Button.enabled(this.item.isEnabled ? this.item.isEnabled : false);
                        ViewStackProcessor.visualState('focused');
                        Button.border({
                            radius: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.ohos_id_corner_radius_clicked'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            width: ImageMenuItem.focusBorderWidth,
                            color: {
                                'id': -1,
                                'type': 10001,
                                params: ['sys.color.ohos_id_color_focused_outline'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            style: BorderStyle.Solid
                        });
                        ViewStackProcessor.visualState('normal');
                        Button.border({
                            radius: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.ohos_id_corner_radius_clicked'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            width: 0
                        });
                        ViewStackProcessor.visualState();
                        Button.onFocus(() => {
                            if (!this.item.isEnabled) {
                                return;
                            }
                            this.isOnFocus = true;
                        });
                        Button.onBlur(() => this.isOnFocus = false);
                        Button.onHover((isOn) => {
                            if (!this.item.isEnabled) {
                                return;
                            }
                            this.isOnHover = isOn;
                        });
                        Button.onKeyEvent((event) => {
                            if (!this.item.isEnabled) {
                                return;
                            }
                            if (event.keyCode !== KeyCode.KEYCODE_ENTER && event.keyCode !== KeyCode.KEYCODE_SPACE) {
                                return;
                            }
                            if (event.type === KeyType.Down) {
                                this.isOnClick = true;
                            }
                            if (event.type === KeyType.Up) {
                                this.isOnClick = false;
                            }
                        });
                        Button.onTouch((event) => {
                            if (!this.item.isEnabled) {
                                return;
                            }
                            if (event.type === TouchType.Down) {
                                this.isOnClick = true;
                            }
                            if (event.type === TouchType.Up || event.type === TouchType.Cancel) {
                                this.isOnClick = false;
                                if (this.fontSize >= this.minFontSize) {
                                    this.dialogController?.close();
                                }
                            }
                        });
                        Button.onClick(() => this.item.isEnabled && this.item.action && this.item.action());
                        Button.gestureModifier(ObservedObject.GetRawObject(this.buttonGestureModifier));
                    }, Button);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.item.symbolStyle) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create();
                                    SymbolGlyph.fontColor([{
                                        'id': -1,
                                        'type': 10001,
                                        params: ['sys.color.font_primary'],
                                        'bundleName': '__harDefaultBundleName__',
                                        'moduleName': '__harDefaultModuleName__',
                                    }]);
                                    SymbolGlyph.attributeModifier.bind(this)(this.item.symbolStyle);
                                    SymbolGlyph.fontSize(ImageMenuItem.imageSize);
                                    SymbolGlyph.draggable(false);
                                    SymbolGlyph.focusable(this.item?.isEnabled);
                                    SymbolGlyph.key(ImageMenuItem.focusablePrefix + this.index);
                                    SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                                }, SymbolGlyph);
                            });
                        } else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    If.create();
                                    if (Util.isSymbolResource(this.item.value)) {
                                        this.ifElseBranchUpdateFunction(0, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                SymbolGlyph.create(this.item.value);
                                                SymbolGlyph.fontColor([{
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.font_primary'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__',
                                                }]);
                                                SymbolGlyph.fontSize(ImageMenuItem.imageSize);
                                                SymbolGlyph.draggable(false);
                                                SymbolGlyph.focusable(this.item?.isEnabled);
                                                SymbolGlyph.key(ImageMenuItem.focusablePrefix + this.index);
                                            }, SymbolGlyph);
                                        });
                                    } else {
                                        this.ifElseBranchUpdateFunction(1, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                Image.create(this.item.value);
                                                Image.draggable(false);
                                                Image.width(ImageMenuItem.imageSize);
                                                Image.height(ImageMenuItem.imageSize);
                                                Image.focusable(this.item.isEnabled);
                                                Image.key(ImageMenuItem.focusablePrefix + this.index);
                                                Image.fillColor({
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.icon_primary'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__',
                                                });
                                            }, Image);
                                        });
                                    }
                                }, If);
                                If.pop();
                            });
                        }
                    }, If);
                    If.pop();
                    Button.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Button.createWithChild({ type: ButtonType.Normal, stateEffect: this.item.isEnabled });
                        Button.accessibilityText(this.getAccessibilityReadText());
                        Button.accessibilityLevel(this.item?.accessibilityLevel ?? 'auto');
                        Button.accessibilityDescription(this.item?.accessibilityDescription);
                        Button.width(ImageMenuItem.imageHotZoneWidth);
                        Button.height(ImageMenuItem.imageHotZoneWidth);
                        Button.borderRadius(ImageMenuItem.buttonBorderRadius);
                        Button.foregroundColor(this.getFgColor());
                        Button.backgroundColor(this.getBgColor());
                        Button.enabled(this.item.isEnabled ? this.item.isEnabled : false);
                        ViewStackProcessor.visualState('focused');
                        Button.border({
                            radius: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.ohos_id_corner_radius_clicked'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            width: ImageMenuItem.focusBorderWidth,
                            color: {
                                'id': -1,
                                'type': 10001,
                                params: ['sys.color.ohos_id_color_focused_outline'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            style: BorderStyle.Solid
                        });
                        ViewStackProcessor.visualState('normal');
                        Button.border({
                            radius: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.ohos_id_corner_radius_clicked'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            width: 0
                        });
                        ViewStackProcessor.visualState();
                        Button.onFocus(() => {
                            if (!this.item.isEnabled) {
                                return;
                            }
                            this.isOnFocus = true;
                        });
                        Button.onBlur(() => this.isOnFocus = false);
                        Button.onHover((isOn) => {
                            if (!this.item.isEnabled) {
                                return;
                            }
                            this.isOnHover = isOn;
                        });
                        Button.onKeyEvent((event) => {
                            if (!this.item.isEnabled) {
                                return;
                            }
                            if (event.keyCode !== KeyCode.KEYCODE_ENTER && event.keyCode !== KeyCode.KEYCODE_SPACE) {
                                return;
                            }
                            if (event.type === KeyType.Down) {
                                this.isOnClick = true;
                            }
                            if (event.type === KeyType.Up) {
                                this.isOnClick = false;
                            }
                        });
                        Button.onTouch((event) => {
                            if (!this.item.isEnabled) {
                                return;
                            }
                            if (event.type === TouchType.Down) {
                                this.isOnClick = true;
                            }
                            if (event.type === TouchType.Up || event.type === TouchType.Cancel) {
                                this.isOnClick = false;
                                if (this.fontSize >= this.minFontSize) {
                                    this.dialogController?.close();
                                }
                            }
                        });
                        Button.onClick(() => this.item.isEnabled && this.item.action && this.item.action());
                    }, Button);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.item.symbolStyle) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create();
                                    SymbolGlyph.fontColor([{
                                        'id': -1,
                                        'type': 10001,
                                        params: ['sys.color.font_primary'],
                                        'bundleName': '__harDefaultBundleName__',
                                        'moduleName': '__harDefaultModuleName__',
                                    }]);
                                    SymbolGlyph.attributeModifier.bind(this)(this.item.symbolStyle);
                                    SymbolGlyph.fontSize(ImageMenuItem.imageSize);
                                    SymbolGlyph.draggable(false);
                                    SymbolGlyph.focusable(this.item?.isEnabled);
                                    SymbolGlyph.key(ImageMenuItem.focusablePrefix + this.index);
                                    SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                                }, SymbolGlyph);
                            });
                        } else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    If.create();
                                    if (Util.isSymbolResource(this.item.value)) {
                                        this.ifElseBranchUpdateFunction(0, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                SymbolGlyph.create(this.item.value);
                                                SymbolGlyph.fontColor([{
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.font_primary'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__',
                                                }]);
                                                SymbolGlyph.fontSize(ImageMenuItem.imageSize);
                                                SymbolGlyph.draggable(false);
                                                SymbolGlyph.focusable(this.item?.isEnabled);
                                                SymbolGlyph.key(ImageMenuItem.focusablePrefix + this.index);
                                            }, SymbolGlyph);
                                        });
                                    } else {
                                        this.ifElseBranchUpdateFunction(1, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                Image.create(this.item.value);
                                                Image.draggable(false);
                                                Image.width(ImageMenuItem.imageSize);
                                                Image.height(ImageMenuItem.imageSize);
                                                Image.focusable(this.item.isEnabled);
                                                Image.key(ImageMenuItem.focusablePrefix + this.index);
                                                Image.fillColor({
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.icon_primary'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__',
                                                });
                                            }, Image);
                                        });
                                    }
                                }, If);
                                If.pop();
                            });
                        }
                    }, If);
                    If.pop();
                    Button.pop();
                });
            }
        }, If);
        If.pop();
    }

    rerender() {
        this.updateDirtyElements();
    }
}

ImageMenuItem.imageSize = '24vp';
ImageMenuItem.imageHotZoneWidth = 48;
ImageMenuItem.buttonBorderRadius = 8;
ImageMenuItem.focusBorderWidth = 2;
ImageMenuItem.focusablePrefix = 'Id-SelectTitleBar-ImageMenuItem-';

class SelectTitleBarDialog extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.selectTitleDialog = {};
        this.callbackId = undefined;
        this.selectTitleBarDialog = '';
        this.mainWindowStage = undefined;
        this.controller = undefined;
        this.minFontSize = 1.75;
        this.maxFontSize = 3.2;
        this.screenWidth = 640;
        this.verticalScreenLines = 6;
        this.horizontalsScreenLines = 1;
        this.__mainWindow = this.createStorageLink('mainWindow', undefined, 'mainWindow');
        this.__fontSize = new ObservedPropertySimplePU(1, this, 'fontSize');
        this.__maxLines = new ObservedPropertySimplePU(1, this, 'maxLines');
        this.__windowStandardHeight = this.createStorageProp('windowStandardHeight', 0, 'windowStandardHeight');
        this.cancel = () => {
        };
        this.confirm = () => {
        };
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }

    setInitiallyProvidedValue(params) {
        if (params.selectTitleDialog !== undefined) {
            this.selectTitleDialog = params.selectTitleDialog;
        }
        if (params.callbackId !== undefined) {
            this.callbackId = params.callbackId;
        }
        if (params.selectTitleBarDialog !== undefined) {
            this.selectTitleBarDialog = params.selectTitleBarDialog;
        }
        if (params.mainWindowStage !== undefined) {
            this.mainWindowStage = params.mainWindowStage;
        }
        if (params.controller !== undefined) {
            this.controller = params.controller;
        }
        if (params.minFontSize !== undefined) {
            this.minFontSize = params.minFontSize;
        }
        if (params.maxFontSize !== undefined) {
            this.maxFontSize = params.maxFontSize;
        }
        if (params.screenWidth !== undefined) {
            this.screenWidth = params.screenWidth;
        }
        if (params.verticalScreenLines !== undefined) {
            this.verticalScreenLines = params.verticalScreenLines;
        }
        if (params.horizontalsScreenLines !== undefined) {
            this.horizontalsScreenLines = params.horizontalsScreenLines;
        }
        if (params.fontSize !== undefined) {
            this.fontSize = params.fontSize;
        }
        if (params.maxLines !== undefined) {
            this.maxLines = params.maxLines;
        }
        if (params.cancel !== undefined) {
            this.cancel = params.cancel;
        }
        if (params.confirm !== undefined) {
            this.confirm = params.confirm;
        }
    }

    updateStateVars(params) {
    }

    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__mainWindow.purgeDependencyOnElmtId(rmElmtId);
        this.__fontSize.purgeDependencyOnElmtId(rmElmtId);
        this.__maxLines.purgeDependencyOnElmtId(rmElmtId);
        this.__windowStandardHeight.purgeDependencyOnElmtId(rmElmtId);
    }

    aboutToBeDeleted() {
        this.__mainWindow.aboutToBeDeleted();
        this.__fontSize.aboutToBeDeleted();
        this.__maxLines.aboutToBeDeleted();
        this.__windowStandardHeight.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    setController(ctr) {
        this.controller = ctr;
    }

    get mainWindow() {
        return this.__mainWindow.get();
    }

    set mainWindow(newValue) {
        this.__mainWindow.set(newValue);
    }

    get fontSize() {
        return this.__fontSize.get();
    }

    set fontSize(newValue) {
        this.__fontSize.set(newValue);
    }

    get maxLines() {
        return this.__maxLines.get();
    }

    set maxLines(newValue) {
        this.__maxLines.set(newValue);
    }

    get windowStandardHeight() {
        return this.__windowStandardHeight.get();
    }

    set windowStandardHeight(newValue) {
        this.__windowStandardHeight.set(newValue);
    }

    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.selectTitleBarDialog) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Column.create();
                        Column.width(this.fontSize === this.maxFontSize ? MAX_DIALOG : MIN_DIALOG);
                        Column.constraintSize({
                            minHeight: this.fontSize === this.maxFontSize ? MAX_DIALOG : MIN_DIALOG
                        });
                        Column.backgroundBlurStyle(BlurStyle.COMPONENT_ULTRA_THICK, undefined,
                            { disableSystemAdaptation: true });
                        Column.shadow(ShadowStyle.OUTER_DEFAULT_LG);
                        Column.borderRadius({
                            'id': -1,
                            'type': 10002,
                            params: ['sys.float.corner_radius_level10'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__',
                        });
                    }, Column);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.selectTitleDialog.symbolStyle) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create();
                                    SymbolGlyph.fontColor([{
                                        'id': -1,
                                        'type': 10001,
                                        params: ['sys.color.font_primary'],
                                        'bundleName': '__harDefaultBundleName__',
                                        'moduleName': '__harDefaultModuleName__',
                                    }]);
                                    SymbolGlyph.attributeModifier.bind(this)(this.selectTitleDialog.symbolStyle);
                                    SymbolGlyph.fontSize(IMAGE_SIZE);
                                    SymbolGlyph.draggable(false);
                                    SymbolGlyph.focusable(this.selectTitleDialog.isEnabled);
                                    SymbolGlyph.margin({
                                        top: {
                                            'id': -1,
                                            'type': 10002,
                                            params: ['sys.float.padding_level24'],
                                            'bundleName': '__harDefaultBundleName__',
                                            'moduleName': '__harDefaultModuleName__',
                                        },
                                        bottom: {
                                            'id': -1,
                                            'type': 10002,
                                            params: ['sys.float.padding_level8'],
                                            'bundleName': '__harDefaultBundleName__',
                                            'moduleName': '__harDefaultModuleName__',
                                        },
                                    });
                                    SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                                }, SymbolGlyph);
                            });
                        } else if (this.selectTitleDialog.value) {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    If.create();
                                    if (Util.isSymbolResource(this.selectTitleDialog.value)) {
                                        this.ifElseBranchUpdateFunction(0, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                SymbolGlyph.create(this.selectTitleDialog.value);
                                                SymbolGlyph.fontColor([{
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.font_primary'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__',
                                                }]);
                                                SymbolGlyph.fontSize(IMAGE_SIZE);
                                                SymbolGlyph.draggable(false);
                                                SymbolGlyph.focusable(this.selectTitleDialog.isEnabled);
                                                SymbolGlyph.margin({
                                                    top: {
                                                        'id': -1,
                                                        'type': 10002,
                                                        params: ['sys.float.padding_level24'],
                                                        'bundleName': '__harDefaultBundleName__',
                                                        'moduleName': '__harDefaultModuleName__',
                                                    },
                                                    bottom: {
                                                        'id': -1,
                                                        'type': 10002,
                                                        params: ['sys.float.padding_level8'],
                                                        'bundleName': '__harDefaultBundleName__',
                                                        'moduleName': '__harDefaultModuleName__',
                                                    },
                                                });
                                            }, SymbolGlyph);
                                        });
                                    } else {
                                        this.ifElseBranchUpdateFunction(1, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                Image.create(this.selectTitleDialog.value);
                                                Image.width(IMAGE_SIZE);
                                                Image.height(IMAGE_SIZE);
                                                Image.margin({
                                                    top: {
                                                        'id': -1,
                                                        'type': 10002,
                                                        params: ['sys.float.padding_level24'],
                                                        'bundleName': '__harDefaultBundleName__',
                                                        'moduleName': '__harDefaultModuleName__',
                                                    },
                                                    bottom: {
                                                        'id': -1,
                                                        'type': 10002,
                                                        params: ['sys.float.padding_level8'],
                                                        'bundleName': '__harDefaultBundleName__',
                                                        'moduleName': '__harDefaultModuleName__',
                                                    },
                                                });
                                                Image.fillColor({
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.icon_primary'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__',
                                                });
                                            }, Image);
                                        });
                                    }
                                }, If);
                                If.pop();
                            });
                        } else {
                            this.ifElseBranchUpdateFunction(2, () => {
                            });
                        }
                    }, If);
                    If.pop();
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Column.create();
                        Column.width('100%');
                        Column.padding({
                            left: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.padding_level4'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            right: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.padding_level4'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                            bottom: {
                                'id': -1,
                                'type': 10002,
                                params: ['sys.float.padding_level12'],
                                'bundleName': '__harDefaultBundleName__',
                                'moduleName': '__harDefaultModuleName__',
                            },
                        });
                    }, Column);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(this.selectTitleBarDialog);
                        Text.fontSize(TEXT_EDITABLE_DIALOG);
                        Text.textOverflow({ overflow: TextOverflow.Ellipsis });
                        Text.maxLines(this.maxLines);
                        Text.width('100%');
                        Text.textAlign(TextAlign.Center);
                        Text.fontColor({
                            'id': -1,
                            'type': 10001,
                            params: ['sys.color.font_primary'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__',
                        });
                    }, Text);
                    Text.pop();
                    Column.pop();
                    Column.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Column.create();
                        Column.width(this.fontSize === this.maxFontSize ? MAX_DIALOG : MIN_DIALOG);
                        Column.constraintSize({
                            minHeight: this.fontSize === this.maxFontSize ? MAX_DIALOG : MIN_DIALOG
                        });
                        Column.backgroundBlurStyle(BlurStyle.COMPONENT_ULTRA_THICK, undefined,
                            { disableSystemAdaptation: true });
                        Column.shadow(ShadowStyle.OUTER_DEFAULT_LG);
                        Column.borderRadius({
                            'id': -1,
                            'type': 10002,
                            params: ['sys.float.corner_radius_level10'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__',
                        });
                        Column.justifyContent(FlexAlign.Center);
                    }, Column);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.selectTitleDialog.symbolStyle) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create();
                                    SymbolGlyph.fontColor([{
                                        'id': -1,
                                        'type': 10001,
                                        params: ['sys.color.font_primary'],
                                        'bundleName': '__harDefaultBundleName__',
                                        'moduleName': '__harDefaultModuleName__',
                                    }]);
                                    SymbolGlyph.attributeModifier.bind(this)(this.selectTitleDialog.symbolStyle);
                                    SymbolGlyph.fontSize(IMAGE_SIZE);
                                    SymbolGlyph.draggable(false);
                                    SymbolGlyph.focusable(this.selectTitleDialog.isEnabled);
                                    SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                                }, SymbolGlyph);
                            });
                        } else if (this.selectTitleDialog.value) {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    If.create();
                                    if (Util.isSymbolResource(this.selectTitleDialog.value)) {
                                        this.ifElseBranchUpdateFunction(0, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                SymbolGlyph.create(this.selectTitleDialog.value);
                                                SymbolGlyph.fontColor([{
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.font_primary'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__',
                                                }]);
                                                SymbolGlyph.fontSize(IMAGE_SIZE);
                                                SymbolGlyph.draggable(false);
                                                SymbolGlyph.focusable(this.selectTitleDialog.isEnabled);
                                            }, SymbolGlyph);
                                        });
                                    } else {
                                        this.ifElseBranchUpdateFunction(1, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                Image.create(this.selectTitleDialog.value);
                                                Image.width(IMAGE_SIZE);
                                                Image.height(IMAGE_SIZE);
                                                Image.fillColor({
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.icon_primary'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__',
                                                });
                                            }, Image);
                                        });
                                    }
                                }, If);
                                If.pop();
                            });
                        } else {
                            this.ifElseBranchUpdateFunction(2, () => {
                            });
                        }
                    }, If);
                    If.pop();
                    Column.pop();
                });
            }
        }, If);
        If.pop();
    }

    async aboutToAppear() {
        try {
            let context = this.getUIContext().getHostContext();
            this.mainWindowStage = context.windowStage.getMainWindowSync();
            let properties = this.mainWindowStage.getWindowProperties();
            let rect = properties.windowRect;
            if (px2vp(rect.height) > this.screenWidth) {
                this.maxLines = this.verticalScreenLines;
            } else {
                this.maxLines = this.horizontalsScreenLines;
            }
        } catch (err) {
            let code = err?.code;
            let message = err?.message;
            hilog.error(0x3900, 'Ace', `Faild to SelectTitleBar getMainWindowSync,code: ${code},message:${message}`);
        }
    }

    rerender() {
        this.updateDirtyElements();
    }
}

class Util {
    static isSymbolResource(resourceStr) {
        if (!Util.isResourceType(resourceStr)) {
            return false;
        }
        let resource = resourceStr;
        return resource.type == RESOURCE_TYPE_SYMBOL;
    }

    static isResourceType(resource) {
        if (!resource) {
            return false;
        }
        if (typeof resource === 'string' || typeof resource === 'undefined') {
            return false;
        }
        return true;
    }
}

export default {
    SelectTitleBar: SelectTitleBar,
};
//# sourceMappingURL=MainPage.js.map