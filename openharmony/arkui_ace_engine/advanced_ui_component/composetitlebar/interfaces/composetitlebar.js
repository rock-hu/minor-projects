/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

if (!('finalizeConstruction' in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, 'finalizeConstruction', () => { });
}
const KeyCode = requireNapi('multimodalInput.keyCode').KeyCode;
const hilog = requireNapi('ohos.hilog');
const i = { 'id': -1, 'type': 40000, params: ['sys.symbol.dot_grid_2x2'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
const j = { 'id': -1, 'type': 40000, params: ['sys.symbol.arrow_left'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
const m = '18.3fp';
const o = '64vp';
const t = '256vp';
const u = '216vp';
const a1 = 40000;
class Util {
    static f1(p1) {
        if (!Util.g1(p1)) {
            return false;
        }
        let resource = p1;
        return resource.type === a1;
    }
    static g1(resource) {
        if (!resource) {
            return false;
        }
        if (typeof resource === 'string' || typeof resource === 'undefined') {
            return false;
        }
        return true;
    }
}
class b1 {
    constructor(controller) {
        this.fontSize = 1;
        this.controller = null;
        this.controller = controller;
    }
    applyGesture(event) {
        if (this.fontSize >= b1.minFontSize) {
            event.addGesture(new LongPressGestureHandler({ repeat: false, duration: b1.h1 })
                .onAction(() => {
                if (event) {
                    this.controller?.open();
                }
            })
                .onActionEnd(() => {
                this.controller?.close();
            }));
        }
        else {
            event.clearGestures();
        }
    }
}
b1.h1 = 500;
b1.minFontSize = 1.75;
class ComposeTitleBar extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.item = undefined;
        this.title = '';
        this.subtitle = '';
        this.menuItems = [];
        this.__titleMaxWidth = new ObservedPropertySimplePU(0, this, 'titleMaxWidth');
        this.__fontSize = new ObservedPropertySimplePU(1, this, 'fontSize');
        this.__uniqueId = new ObservedPropertySimplePU(-1, this, 'uniqueId');
        this.addProvidedVar('uniqueId', this.__uniqueId, false);
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params) {
        if (params.item !== undefined) {
            this.item = params.item;
        }
        if (params.title !== undefined) {
            this.title = params.title;
        }
        if (params.subtitle !== undefined) {
            this.subtitle = params.subtitle;
        }
        if (params.menuItems !== undefined) {
            this.menuItems = params.menuItems;
        }
        if (params.titleMaxWidth !== undefined) {
            this.titleMaxWidth = params.titleMaxWidth;
        }
        if (params.fontSize !== undefined) {
            this.fontSize = params.fontSize;
        }
        if (params.uniqueId !== undefined) {
            this.uniqueId = params.uniqueId;
        }
    }
    updateStateVars(params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__titleMaxWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__fontSize.purgeDependencyOnElmtId(rmElmtId);
        this.__uniqueId.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__titleMaxWidth.aboutToBeDeleted();
        this.__fontSize.aboutToBeDeleted();
        this.__uniqueId.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    get titleMaxWidth() {
        return this.__titleMaxWidth.get();
    }
    set titleMaxWidth(newValue) {
        this.__titleMaxWidth.set(newValue);
    }
    get fontSize() {
        return this.__fontSize.get();
    }
    set fontSize(newValue) {
        this.__fontSize.set(newValue);
    }
    get uniqueId() {
        return this.__uniqueId.get();
    }
    set uniqueId(newValue) {
        this.__uniqueId.set(newValue);
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Flex.create({
                justifyContent: FlexAlign.SpaceBetween,
                alignItems: ItemAlign.Stretch
            });
            Flex.onAppear(() => {
                this.uniqueId = this.getUIContext().getFrameNodeByUniqueId(this.getUniqueId())?.getFirstChild()?.getUniqueId();
            });
            Flex.width('100%');
            Flex.height(ComposeTitleBar.totalHeight);
            Flex.backgroundColor({ 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_background'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
            Flex.onAreaChange((j8, newValue) => {
                let k8 = Number(newValue.width);
                if (this.menuItems !== undefined) {
                    let l8 = this.menuItems.length;
                    if (l8 >= c1.maxCountOfVisibleItems) {
                        k8 = k8 - d1.imageHotZoneWidth * c1.maxCountOfVisibleItems;
                    }
                    else if (l8 > 0) {
                        k8 = k8 - d1.imageHotZoneWidth * l8;
                    }
                }
                this.titleMaxWidth = k8;
                this.titleMaxWidth -= ComposeTitleBar.leftPadding;
                this.titleMaxWidth -= d1.imageHotZoneWidth;
                if (this.item !== undefined) {
                    this.titleMaxWidth -= ComposeTitleBar.portraitImageLeftPadding +
                        ComposeTitleBar.portraitImageSize +
                        ComposeTitleBar.portraitImageRightPadding;
                }
                this.titleMaxWidth -= ComposeTitleBar.rightPadding;
            });
        }, Flex);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.margin({ left: { 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_default_padding_start'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' } });
        }, Row);
        {
            this.observeComponentCreation2((elmtId, isInitialRender) => {
                if (isInitialRender) {
                    let componentCall = new d1(this, {
                        item: {
                            value: j,
                            isEnabled: true,
                            action: () => this.getUIContext()?.getRouter()?.back()
                        },
                        index: -1,
                        itemIndex: -1
                    }, undefined, elmtId, () => { }, { page: 'library/src/main/ets/components/MainPage.ets', line: 113, m1: 9 });
                    ViewPU.create(componentCall);
                    let paramsLambda = () => {
                        return {
                            item: {
                                value: j,
                                isEnabled: true,
                                action: () => this.getUIContext()?.getRouter()?.back()
                            },
                            index: -1,
                            itemIndex: -1
                        };
                    };
                    componentCall.paramsGenerator_ = paramsLambda;
                }
                else {
                    this.updateStateVarsOfChildByElmtId(elmtId, {});
                }
            }, { name: 'ImageMenuItem' });
        }
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.accessibilityGroup(true);
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.item !== undefined) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Image.create(this.item.value);
                        Image.width(ComposeTitleBar.portraitImageSize);
                        Image.height(ComposeTitleBar.portraitImageSize);
                        Image.margin({
                            left: { 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_text_paragraph_margin_xs'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                            right: { 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_text_paragraph_margin_m'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' }
                        });
                        Image.focusable(false);
                        Image.borderRadius(d1.buttonBorderRadius);
                    }, Image);
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.justifyContent(FlexAlign.Start);
            Column.alignItems(HorizontalAlign.Start);
            Column.constraintSize({ maxWidth: this.titleMaxWidth });
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.title !== undefined) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Row.create();
                        Row.justifyContent(FlexAlign.Start);
                    }, Row);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(this.title);
                        Text.fontWeight(FontWeight.Medium);
                        Text.fontSize({ 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_text_size_headline8'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                        Text.fontColor({ 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_titlebar_text'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                        Text.maxLines(this.subtitle !== undefined ? 1 : 2);
                        Text.textOverflow({ overflow: TextOverflow.Ellipsis });
                        Text.constraintSize({ maxWidth: this.titleMaxWidth });
                    }, Text);
                    Text.pop();
                    Row.pop();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
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
                    }, Row);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(this.subtitle);
                        Text.fontSize({ 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_text_size_over_line'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                        Text.fontColor({ 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_titlebar_subtitle_text'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                        Text.maxLines(1);
                        Text.textOverflow({ overflow: TextOverflow.Ellipsis });
                        Text.constraintSize({ maxWidth: this.titleMaxWidth });
                    }, Text);
                    Text.pop();
                    Row.pop();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Column.pop();
        Row.pop();
        Row.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.menuItems !== undefined && this.menuItems.length > 0) {
                this.ifElseBranchUpdateFunction(0, () => {
                    {
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            if (isInitialRender) {
                                let componentCall = new c1(this, { menuItems: this.menuItems, index: 1 + ComposeTitleBar.instanceCount++ }, undefined, elmtId, () => { }, { page: 'library/src/main/ets/components/MainPage.ets', line: 170, m1: 9 });
                                ViewPU.create(componentCall);
                                let paramsLambda = () => {
                                    return {
                                        menuItems: this.menuItems,
                                        index: 1 + ComposeTitleBar.instanceCount++
                                    };
                                };
                                componentCall.paramsGenerator_ = paramsLambda;
                            }
                            else {
                                this.updateStateVarsOfChildByElmtId(elmtId, {});
                            }
                        }, { name: 'CollapsibleMenuSection' });
                    }
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Flex.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
}
ComposeTitleBar.totalHeight = 56;
ComposeTitleBar.leftPadding = 12;
ComposeTitleBar.rightPadding = 12;
ComposeTitleBar.portraitImageSize = 40;
ComposeTitleBar.portraitImageLeftPadding = 4;
ComposeTitleBar.portraitImageRightPadding = 16;
ComposeTitleBar.instanceCount = 0;
class c1 extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.menuItems = [];
        this.item = {
            value: i,
            label: { 'id': -1, 'type': 10003, params: ['sys.string.ohos_toolbar_more'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
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
                let jsDialog = new e1(this, {
                    cancel: () => {
                    },
                    confirm: () => {
                    },
                    itemComposeTitleDialog: this.item,
                    composeTitleBarDialog: this.item.label ? this.item.label : '',
                    fontSize: this.fontSize,
                }, undefined, -1, () => { }, { page: 'library/src/main/ets/components/MainPage.ets', line: 224, m1: 14 });
                jsDialog.setController(this.dialogController);
                ViewPU.create(jsDialog);
                let paramsLambda = () => {
                    return {
                        cancel: () => {
                        },
                        confirm: () => {
                        },
                        itemComposeTitleDialog: this.item,
                        composeTitleBarDialog: this.item.label ? this.item.label : '',
                        fontSize: this.fontSize
                    };
                };
                jsDialog.paramsGenerator_ = paramsLambda;
            },
            maskColor: Color.Transparent,
            isModal: true,
            customStyle: true
        }, this);
        this.__buttonGestureModifier = new ObservedPropertyObjectPU(new b1(this.dialogController), this, 'buttonGestureModifier');
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
        return this.isMoreIconOnClick ? { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_titlebar_icon_pressed'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' } : { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_titlebar_icon'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
    }
    getMoreIconBgColor() {
        if (this.isMoreIconOnClick) {
            return { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_click_effect'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
        }
        else if (this.isMoreIconOnHover) {
            return { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_hover'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
        }
        else {
            return Color.Transparent;
        }
    }
    aboutToAppear() {
        try {
            let l5 = this.getUIContext();
            this.isFollowingSystemFontScale = l5.isFollowingSystemFontScale();
            this.maxFontScale = l5.getMaxFontScale();
        }
        catch (err) {
            let code = err.code;
            let message = err.message;
            hilog.error(0x3900, 'ComposeTitleBar', `Failed to init fontsizescale info, cause, code: ${code}, message: ${message}`);
        }
        if (this.menuItems) {
            this.menuItems.forEach((item, index) => {
                if (item.isEnabled && this.firstFocusableIndex === -1 &&
                    index > c1.maxCountOfVisibleItems - 2) {
                    this.firstFocusableIndex = this.index * 1000 + index + 1;
                }
            });
        }
        this.fontSize = this.decideFontScale();
    }
    decideFontScale() {
        try {
            let k5 = this.getUIContext();
            this.systemFontScale = k5.getHostContext()?.config?.fontSizeScale ?? 1;
            if (!this.isFollowingSystemFontScale) {
                return 1;
            }
            return Math.min(this.systemFontScale, this.maxFontScale);
        }
        catch (j5) {
            let code = j5.code;
            let message = j5.message;
            hilog.error(0x3900, 'ComposeTitleBar', `Faild to decideFontScale,cause, code: ${code}, message: ${message}`);
            return 1;
        }
    }
    onFontSizeUpdated() {
        this.buttonGestureModifier.fontSize = this.fontSize;
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.height('100%');
            Column.margin({ right: { 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_default_padding_end'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' } });
            Column.justifyContent(FlexAlign.Center);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.menuItems) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.menuItems.length <= c1.maxCountOfVisibleItems) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    ForEach.create();
                                    const forEachItemGenFunction = (_item, index) => {
                                        const item = _item;
                                        {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                if (isInitialRender) {
                                                    let componentCall = new d1(this, { item: item, index: this.index * 1000 + index + 1, itemIndex: index }, undefined, elmtId, () => { }, { page: 'library/src/main/ets/components/MainPage.ets', line: 304, m1: 15 });
                                                    ViewPU.create(componentCall);
                                                    let paramsLambda = () => {
                                                        return {
                                                            item: item,
                                                            index: this.index * 1000 + index + 1,
                                                            itemIndex: index
                                                        };
                                                    };
                                                    componentCall.paramsGenerator_ = paramsLambda;
                                                }
                                                else {
                                                    this.updateStateVarsOfChildByElmtId(elmtId, {});
                                                }
                                            }, { name: 'ImageMenuItem' });
                                        }
                                    };
                                    this.forEachUpdateFunction(elmtId, this.menuItems, forEachItemGenFunction, undefined, true, false);
                                }, ForEach);
                                ForEach.pop();
                            });
                        }
                        else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    ForEach.create();
                                    const forEachItemGenFunction = (_item, index) => {
                                        const item = _item;
                                        {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                if (isInitialRender) {
                                                    let componentCall = new d1(this, { item: item, index: this.index * 1000 + index + 1, itemIndex: index }, undefined, elmtId, () => { }, { page: 'library/src/main/ets/components/MainPage.ets', line: 309, m1: 17 });
                                                    ViewPU.create(componentCall);
                                                    let paramsLambda = () => {
                                                        return {
                                                            item: item,
                                                            index: this.index * 1000 + index + 1,
                                                            itemIndex: index
                                                        };
                                                    };
                                                    componentCall.paramsGenerator_ = paramsLambda;
                                                }
                                                else {
                                                    this.updateStateVarsOfChildByElmtId(elmtId, {});
                                                }
                                            }, { name: 'ImageMenuItem' });
                                        }
                                    };
                                    this.forEachUpdateFunction(elmtId, this.menuItems.slice(0, c1.maxCountOfVisibleItems - 1), forEachItemGenFunction, undefined, true, false);
                                }, ForEach);
                                ForEach.pop();
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    Button.createWithChild({ type: ButtonType.Normal, stateEffect: true });
                                    Button.accessibilityText({ 'id': -1, 'type': 10003, params: ['sys.string.ohos_toolbar_more'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                                    Button.width(d1.imageHotZoneWidth);
                                    Button.height(d1.imageHotZoneWidth);
                                    Button.borderRadius(d1.buttonBorderRadius);
                                    Button.foregroundColor(this.getMoreIconFgColor());
                                    Button.backgroundColor(this.getMoreIconBgColor());
                                    ViewStackProcessor.visualState('focused');
                                    Button.border({
                                        radius: { 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_corner_radius_clicked'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                                        width: d1.focusBorderWidth,
                                        color: { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_focused_outline'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                                        style: BorderStyle.Solid
                                    });
                                    ViewStackProcessor.visualState('normal');
                                    Button.border({
                                        radius: { 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_corner_radius_clicked'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                                        width: 0
                                    });
                                    ViewStackProcessor.visualState();
                                    Button.onFocus(() => this.isMoreIconOnFocus = true);
                                    Button.onBlur(() => this.isMoreIconOnFocus = false);
                                    Button.onHover((isOn) => this.isMoreIconOnHover = isOn);
                                    Button.onKeyEvent((event) => {
                                        if (event.keyCode !== KeyCode.KEYCODE_ENTER &&
                                            event.keyCode !== KeyCode.KEYCODE_SPACE) {
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
                                    SymbolGlyph.create(i);
                                    SymbolGlyph.fontSize(`${d1.imageSize}vp`);
                                    SymbolGlyph.fontColor([{ 'id': -1, 'type': 10001, params: ['sys.color.icon_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' }]);
                                    SymbolGlyph.draggable(false);
                                    SymbolGlyph.focusable(true);
                                }, SymbolGlyph);
                                Button.pop();
                            });
                        }
                    }, If);
                    If.pop();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Row.pop();
        Column.pop();
    }
    popupBuilder(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.width(d1.imageHotZoneWidth +
                c1.focusPadding * c1.marginsNum);
            Column.margin({ top: c1.focusPadding, bottom: c1.focusPadding });
            Column.onAppear(() => {
                focusControl.requestFocus(d1.focusablePrefix +
                    this.firstFocusableIndex);
            });
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.menuItems) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        ForEach.create();
                        const forEachItemGenFunction = (_item, index) => {
                            const item = _item;
                            {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    if (isInitialRender) {
                                        let componentCall = new d1(this, {
                                            item: item,
                                            index: this.index * 1000 +
                                                c1.maxCountOfVisibleItems + index,
                                            isPopup: true
                                        }, undefined, elmtId, () => { }, { page: 'library/src/main/ets/components/MainPage.ets', line: 397, m1: 13 });
                                        ViewPU.create(componentCall);
                                        let paramsLambda = () => {
                                            return {
                                                item: item,
                                                index: this.index * 1000 +
                                                    c1.maxCountOfVisibleItems + index,
                                                isPopup: true
                                            };
                                        };
                                        componentCall.paramsGenerator_ = paramsLambda;
                                    }
                                    else {
                                        this.updateStateVarsOfChildByElmtId(elmtId, {});
                                    }
                                }, { name: 'ImageMenuItem' });
                            }
                        };
                        this.forEachUpdateFunction(elmtId, this.menuItems.slice(c1.maxCountOfVisibleItems - 1, this.menuItems.length), forEachItemGenFunction, undefined, true, false);
                    }, ForEach);
                    ForEach.pop();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Column.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
}
c1.maxCountOfVisibleItems = 3;
c1.focusPadding = 4;
c1.marginsNum = 2;
class d1 extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.item = {};
        this.index = 0;
        this.itemIndex = 0;
        this.minFontSize = 1.75;
        this.isFollowingSystemFontScale = false;
        this.maxFontScale = 1;
        this.systemFontScale = 1;
        this.isPopup = false;
        this.__isOnFocus = new ObservedPropertySimplePU(false, this, 'isOnFocus');
        this.__isOnHover = new ObservedPropertySimplePU(false, this, 'isOnHover');
        this.__isOnClick = new ObservedPropertySimplePU(false, this, 'isOnClick');
        this.__fontSize = new SynchedPropertySimpleOneWayPU(params.fontSize, this, 'fontSize');
        this.__parentParentUniqueId = this.initializeConsume('uniqueId', 'parentParentUniqueId');
        this.dialogController = new CustomDialogController({
            builder: () => {
                let jsDialog = new e1(this, {
                    cancel: () => {
                    },
                    confirm: () => {
                    },
                    itemComposeTitleDialog: this.item,
                    composeTitleBarDialog: this.item.label ? this.item.label : this.textDialog(),
                    fontSize: this.fontSize,
                }, undefined, -1, () => { }, { page: 'library/src/main/ets/components/MainPage.ets', line: 435, m1: 14 });
                jsDialog.setController(this.dialogController);
                ViewPU.create(jsDialog);
                let paramsLambda = () => {
                    return {
                        cancel: () => {
                        },
                        confirm: () => {
                        },
                        itemComposeTitleDialog: this.item,
                        composeTitleBarDialog: this.item.label ? this.item.label : this.textDialog(),
                        fontSize: this.fontSize
                    };
                };
                jsDialog.paramsGenerator_ = paramsLambda;
            },
            maskColor: Color.Transparent,
            isModal: true,
            customStyle: true
        }, this);
        this.__buttonGestureModifier = new ObservedPropertyObjectPU(new b1(this.dialogController), this, 'buttonGestureModifier');
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
        if (params.itemIndex !== undefined) {
            this.itemIndex = params.itemIndex;
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
        this.__parentParentUniqueId.purgeDependencyOnElmtId(rmElmtId);
        this.__buttonGestureModifier.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__isOnFocus.aboutToBeDeleted();
        this.__isOnHover.aboutToBeDeleted();
        this.__isOnClick.aboutToBeDeleted();
        this.__fontSize.aboutToBeDeleted();
        this.__parentParentUniqueId.aboutToBeDeleted();
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
    get parentParentUniqueId() {
        return this.__parentParentUniqueId.get();
    }
    set parentParentUniqueId(newValue) {
        this.__parentParentUniqueId.set(newValue);
    }
    get buttonGestureModifier() {
        return this.__buttonGestureModifier.get();
    }
    set buttonGestureModifier(newValue) {
        this.__buttonGestureModifier.set(newValue);
    }
    textDialog() {
        if (this.item.value === i) {
            return { 'id': -1, 'type': 10003, params: ['sys.string.ohos_toolbar_more'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
        }
        else if (this.item.value === j) {
            return { 'id': -1, 'type': 10003, params: ['sys.string.icon_back'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
        }
        else {
            return this.item.label ? this.item.label : '';
        }
    }
    toStringFormat(resource) {
        if (typeof resource === 'string') {
            return resource;
        }
        else if (typeof resource === 'undefined') {
            return '';
        }
        else {
            let i5 = '';
            try {
                i5 = getContext()?.resourceManager?.getStringSync(resource);
            }
            catch (err) {
                let code = err?.code;
                let message = err?.message;
                hilog.error(0x3900, 'Ace', `Faild to ComposeTitleBar toStringFormat,code: ${code},message:${message}`);
            }
            return i5;
        }
    }
    getAccessibilityReadText() {
        if (this.item.value === j) {
            return getContext()?.resourceManager?.getStringByNameSync('icon_back');
        }
        else if (this.item.value === i) {
            return getContext()?.resourceManager?.getStringByNameSync('ohos_toolbar_more');
        }
        else if (this.item.accessibilityText) {
            return this.toStringFormat(this.item.accessibilityText);
        }
        else if (this.item.label) {
            return this.toStringFormat(this.item.label);
        }
        return ' ';
    }
    onPlaceChildren(selfLayoutInfo, children, constraint) {
        children.forEach((child) => {
            child.layout({ x: 0, y: 0 });
        });
        this.fontSize = this.decideFontScale();
    }
    getFgColor() {
        return this.isOnClick
            ? { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_titlebar_icon_pressed'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' } : { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_titlebar_icon'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
    }
    getBgColor() {
        if (this.isOnClick) {
            return { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_click_effect'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
        }
        else if (this.isOnHover) {
            return { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_hover'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' };
        }
        else {
            return Color.Transparent;
        }
    }
    aboutToAppear() {
        try {
            let h5 = this.getUIContext();
            this.isFollowingSystemFontScale = h5.isFollowingSystemFontScale();
            this.maxFontScale = h5.getMaxFontScale();
        }
        catch (err) {
            let code = err.code;
            let message = err.message;
            hilog.error(0x3900, 'ComposeTitleBar', `Failed to init fontsizescale info, cause, code: ${code}, message: ${message}`);
        }
        this.fontSize = this.decideFontScale();
    }
    onFontSizeUpdated() {
        this.buttonGestureModifier.fontSize = this.fontSize;
    }
    decideFontScale() {
        try {
            let g5 = this.getUIContext();
            this.systemFontScale = g5.getHostContext()?.config?.fontSizeScale ?? 1;
            if (!this.isFollowingSystemFontScale) {
                return 1;
            }
            return Math.min(this.systemFontScale, this.maxFontScale);
        }
        catch (f5) {
            let code = f5.code;
            let message = f5.message;
            hilog.error(0x3900, 'ComposeTitleBar', `Faild to decideFontScale,cause, code: ${code}, message: ${message}`);
            return 1;
        }
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Button.createWithChild({ type: ButtonType.Normal, stateEffect: this.item.isEnabled });
            Button.id(`ComposeTitleBar_ImageMenuItem_${this.parentParentUniqueId}_${this.itemIndex}`);
            Button.accessibilityText(this.getAccessibilityReadText());
            Button.accessibilityLevel(this.item?.accessibilityLevel ?? 'auto');
            Button.accessibilityDescription(this.toStringFormat(this.item?.accessibilityDescription));
            Button.enabled(this.item.isEnabled ? this.item.isEnabled : false);
            Button.width(d1.imageHotZoneWidth);
            Button.height(d1.imageHotZoneWidth);
            Button.borderRadius(d1.buttonBorderRadius);
            Button.foregroundColor(this.getFgColor());
            Button.backgroundColor(this.getBgColor());
            ViewStackProcessor.visualState('focused');
            Button.border({
                radius: { 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_corner_radius_clicked'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                width: d1.focusBorderWidth,
                color: { 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_focused_outline'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                style: BorderStyle.Solid
            });
            ViewStackProcessor.visualState('normal');
            Button.border({
                radius: { 'id': -1, 'type': 10002, params: ['sys.float.ohos_id_corner_radius_clicked'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                width: 0
            });
            ViewStackProcessor.visualState();
            Button.onFocus(() => {
                if (!this.item?.isEnabled) {
                    return;
                }
                this.isOnFocus = true;
            });
            Button.onBlur(() => this.isOnFocus = false);
            Button.onHover((isOn) => {
                if (!this.item?.isEnabled) {
                    return;
                }
                this.isOnHover = isOn;
            });
            Button.onKeyEvent((event) => {
                if (!this.item?.isEnabled) {
                    return;
                }
                if (event.keyCode !== KeyCode.KEYCODE_ENTER &&
                    event.keyCode !== KeyCode.KEYCODE_SPACE) {
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
                if (!this.item?.isEnabled) {
                    return;
                }
                if (event.type === TouchType.Down) {
                    this.isOnClick = true;
                }
                if (event.type === TouchType.Up || event.type === TouchType.Cancel) {
                    this.isOnClick = false;
                    if (this.fontSize >= this.minFontSize && this.isPopup === false) {
                        this.dialogController?.close();
                    }
                }
            });
            Button.onClick(() => {
                if (this.item) {
                    return this.item.isEnabled && this.item.action?.();
                }
            });
            Button.gestureModifier(ObservedObject.GetRawObject(this.buttonGestureModifier));
        }, Button);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.item?.symbolStyle) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        SymbolGlyph.create();
                        SymbolGlyph.fontColor([{ 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_text_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' }]);
                        SymbolGlyph.attributeModifier.bind(this)(this.item?.symbolStyle);
                        SymbolGlyph.fontSize(`${d1.imageSize}vp`);
                        SymbolGlyph.effectStrategy(SymbolEffectStrategy.NONE);
                        SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                        SymbolGlyph.draggable(false);
                        SymbolGlyph.focusable(this.item?.isEnabled);
                        SymbolGlyph.key(d1.focusablePrefix + this.index);
                    }, SymbolGlyph);
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (Util.f1(this.item.value)) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create(this.item.value);
                                    SymbolGlyph.fontSize(`${d1.imageSize}vp`);
                                    SymbolGlyph.fontColor([{ 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_text_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' }]);
                                    SymbolGlyph.draggable(false);
                                    SymbolGlyph.focusable(this.item?.isEnabled);
                                    SymbolGlyph.key(d1.focusablePrefix + this.index);
                                }, SymbolGlyph);
                            });
                        }
                        else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    Image.create(this.item?.value);
                                    Image.matchTextDirection(this.item?.value === j ? true : false);
                                    Image.width(d1.imageSize);
                                    Image.draggable(false);
                                    Image.height(d1.imageSize);
                                    Image.focusable(this.item?.isEnabled);
                                    Image.key(d1.focusablePrefix + this.index);
                                    Image.fillColor({ 'id': -1, 'type': 10001, params: ['sys.color.ohos_id_color_text_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
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
    }
    rerender() {
        this.updateDirtyElements();
    }
}
d1.imageSize = 24;
d1.imageHotZoneWidth = 48;
d1.buttonBorderRadius = 8;
d1.focusBorderWidth = 2;
d1.focusablePrefix = 'Id-ComposeTitleBar-ImageMenuItem-';
class e1 extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.itemComposeTitleDialog = {};
        this.callbackId = undefined;
        this.composeTitleBarDialog = '';
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
        if (params.itemComposeTitleDialog !== undefined) {
            this.itemComposeTitleDialog = params.itemComposeTitleDialog;
        }
        if (params.callbackId !== undefined) {
            this.callbackId = params.callbackId;
        }
        if (params.composeTitleBarDialog !== undefined) {
            this.composeTitleBarDialog = params.composeTitleBarDialog;
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
            if (this.composeTitleBarDialog) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Column.create();
                        Column.width(this.fontSize === this.maxFontSize ? t : u);
                        Column.constraintSize({ minHeight: this.fontSize === this.maxFontSize ? t : u });
                        Column.backgroundBlurStyle(BlurStyle.COMPONENT_ULTRA_THICK);
                        Column.shadow(ShadowStyle.OUTER_DEFAULT_LG);
                        Column.borderRadius({ 'id': -1, 'type': 10002, params: ['sys.float.corner_radius_level10'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                    }, Column);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.itemComposeTitleDialog.symbolStyle) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create();
                                    SymbolGlyph.fontColor([{ 'id': -1, 'type': 10001, params: ['sys.color.icon_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' }]);
                                    SymbolGlyph.attributeModifier.bind(this)(this.itemComposeTitleDialog.symbolStyle);
                                    SymbolGlyph.fontSize(o);
                                    SymbolGlyph.effectStrategy(SymbolEffectStrategy.NONE);
                                    SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                                    SymbolGlyph.margin({
                                        top: { 'id': -1, 'type': 10002, params: ['sys.float.padding_level24'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                                        bottom: { 'id': -1, 'type': 10002, params: ['sys.float.padding_level8'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                                    });
                                }, SymbolGlyph);
                            });
                        }
                        else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    If.create();
                                    if (Util.f1(this.itemComposeTitleDialog.value)) {
                                        this.ifElseBranchUpdateFunction(0, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                SymbolGlyph.create(this.itemComposeTitleDialog.value);
                                                SymbolGlyph.fontSize(o);
                                                SymbolGlyph.fontColor([{ 'id': -1, 'type': 10001, params: ['sys.color.icon_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' }]);
                                                SymbolGlyph.margin({
                                                    top: { 'id': -1, 'type': 10002, params: ['sys.float.padding_level24'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                                                    bottom: { 'id': -1, 'type': 10002, params: ['sys.float.padding_level8'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                                                });
                                            }, SymbolGlyph);
                                        });
                                    }
                                    else {
                                        this.ifElseBranchUpdateFunction(1, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                Image.create(this.itemComposeTitleDialog.value);
                                                Image.width(o);
                                                Image.height(o);
                                                Image.margin({
                                                    top: { 'id': -1, 'type': 10002, params: ['sys.float.padding_level24'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                                                    bottom: { 'id': -1, 'type': 10002, params: ['sys.float.padding_level8'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                                                });
                                                Image.fillColor({ 'id': -1, 'type': 10001, params: ['sys.color.icon_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                                            }, Image);
                                        });
                                    }
                                }, If);
                                If.pop();
                            });
                        }
                    }, If);
                    If.pop();
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Column.create();
                        Column.width('100%');
                        Column.padding({
                            left: { 'id': -1, 'type': 10002, params: ['sys.float.padding_level4'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                            right: { 'id': -1, 'type': 10002, params: ['sys.float.padding_level4'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                            bottom: { 'id': -1, 'type': 10002, params: ['sys.float.padding_level12'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' },
                        });
                    }, Column);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(this.composeTitleBarDialog);
                        Text.fontSize(m);
                        Text.textOverflow({ overflow: TextOverflow.Ellipsis });
                        Text.maxLines(this.maxLines);
                        Text.width('100%');
                        Text.textAlign(TextAlign.Center);
                        Text.fontColor({ 'id': -1, 'type': 10001, params: ['sys.color.font_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                    }, Text);
                    Text.pop();
                    Column.pop();
                    Column.pop();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Column.create();
                        Column.width(this.fontSize === this.maxFontSize ? t : u);
                        Column.constraintSize({ minHeight: this.fontSize === this.maxFontSize ? t : u });
                        Column.backgroundBlurStyle(BlurStyle.COMPONENT_ULTRA_THICK);
                        Column.shadow(ShadowStyle.OUTER_DEFAULT_LG);
                        Column.borderRadius({ 'id': -1, 'type': 10002, params: ['sys.float.corner_radius_level10'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                        Column.justifyContent(FlexAlign.Center);
                    }, Column);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.itemComposeTitleDialog.symbolStyle) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create();
                                    SymbolGlyph.fontColor([{ 'id': -1, 'type': 10001, params: ['sys.color.icon_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' }]);
                                    SymbolGlyph.attributeModifier.bind(this)(this.itemComposeTitleDialog.symbolStyle);
                                    SymbolGlyph.fontSize(o);
                                    SymbolGlyph.effectStrategy(SymbolEffectStrategy.NONE);
                                    SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                                }, SymbolGlyph);
                            });
                        }
                        else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    If.create();
                                    if (Util.f1(this.itemComposeTitleDialog.value)) {
                                        this.ifElseBranchUpdateFunction(0, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                SymbolGlyph.create(this.itemComposeTitleDialog.value);
                                                SymbolGlyph.fontSize(o);
                                                SymbolGlyph.fontColor([{ 'id': -1, 'type': 10001, params: ['sys.color.icon_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' }]);
                                            }, SymbolGlyph);
                                        });
                                    }
                                    else {
                                        this.ifElseBranchUpdateFunction(1, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                Image.create(this.itemComposeTitleDialog.value);
                                                Image.width(o);
                                                Image.height(o);
                                                Image.fillColor({ 'id': -1, 'type': 10001, params: ['sys.color.icon_primary'], 'bundleName': '__harDefaultBundleName__', 'moduleName': '__harDefaultModuleName__' });
                                            }, Image);
                                        });
                                    }
                                }, If);
                                If.pop();
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
        let context = this.getUIContext().getHostContext();
        this.mainWindowStage = context.windowStage.getMainWindowSync();
        let properties = this.mainWindowStage.getWindowProperties();
        let rect = properties.windowRect;
        if (px2vp(rect.height) > this.screenWidth) {
            this.maxLines = this.verticalScreenLines;
        }
        else {
            this.maxLines = this.horizontalsScreenLines;
        }
    }
    rerender() {
        this.updateDirtyElements();
    }
}
export default { ComposeTitleBar };
