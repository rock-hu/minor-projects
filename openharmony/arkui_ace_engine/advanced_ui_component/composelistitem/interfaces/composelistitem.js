/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
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
const LengthMetrics = requireNapi('arkui.node').LengthMetrics;
const hilog = requireNapi('hilog');
const measure = requireNapi('measure');

export var IconType;
(function (IconType) {
    IconType[IconType['BADGE'] = 1] = 'BADGE';
    IconType[IconType['NORMAL_ICON'] = 2] = 'NORMAL_ICON';
    IconType[IconType['SYSTEM_ICON'] = 3] = 'SYSTEM_ICON';
    IconType[IconType['HEAD_SCULPTURE'] = 4] = 'HEAD_SCULPTURE';
    IconType[IconType['APP_ICON'] = 5] = 'APP_ICON';
    IconType[IconType['PREVIEW'] = 6] = 'PREVIEW';
    IconType[IconType['LONGITUDINAL'] = 7] = 'LONGITUDINAL';
    IconType[IconType['VERTICAL'] = 8] = 'VERTICAL';
})(IconType || (IconType = {}));
var FontSizeScaleLevel;
(function (FontSizeScaleLevel) {
    FontSizeScaleLevel[FontSizeScaleLevel['LEVEL1'] = 1.75] = 'LEVEL1';
    FontSizeScaleLevel[FontSizeScaleLevel['LEVEL2'] = 2] = 'LEVEL2';
    FontSizeScaleLevel[FontSizeScaleLevel['LEVEL3'] = 3.2] = 'LEVEL3';
})(FontSizeScaleLevel || (FontSizeScaleLevel = {}));
var ItemHeight;
(function (ItemHeight) {
    ItemHeight[ItemHeight['FIRST_HEIGHT'] = 48] = 'FIRST_HEIGHT';
    ItemHeight[ItemHeight['SECOND_HEIGHT'] = 56] = 'SECOND_HEIGHT';
    ItemHeight[ItemHeight['THIRD_HEIGHT'] = 64] = 'THIRD_HEIGHT';
    ItemHeight[ItemHeight['FOURTH_HEIGHT'] = 72] = 'FOURTH_HEIGHT';
    ItemHeight[ItemHeight['FIFTH_HEIGHT'] = 96] = 'FIFTH_HEIGHT';
})(ItemHeight || (ItemHeight = {}));
const TEXT_MAX_LINE = 1;
const ITEM_BORDER_SHOWN = 2;
const TEXT_COLUMN_SPACE = 4;
const TEXT_SAFE_MARGIN = 8;
const LISTITEM_PADDING = 6;
const SWITCH_PADDING = 4;
const STACK_PADDING = 4;
const BADGE_SIZE = 8;
const SMALL_ICON_SIZE = 16;
const SYSTEM_ICON_SIZE = 24;
const TEXT_ARROW_HEIGHT = 32;
const SAFE_LIST_PADDING = 32;
const HEADSCULPTURE_SIZE = 40;
const BUTTON_SIZE = 28;
const APP_ICON_SIZE = 64;
const PREVIEW_SIZE = 96;
const LONGITUDINAL_SIZE = 96;
const VERTICAL_SIZE = 96;
const NORMAL_ITEM_ROW_SPACE = 16;
const SPECIAL_ITEM_ROW_SPACE = 0;
const SPECIAL_ICON_SIZE = 0;
const DEFAULT_ROW_SPACE = 0;
const SPECICAL_ROW_SPACE = 4;
const OPERATEITEM_ICONLIKE_SIZE = 24;
const OPERATEITEM_SELECTIONBOX_PADDING_SIZE = 2;
const OPERATEITEM_ARROW_WIDTH = 12;
const OPERATEITEM_ICON_CLICKABLE_SIZE = 40;
const OPERATEITEM_IMAGE_SIZE = 48;
const RIGHT_CONTENT_NULL_RIGHTWIDTH = '0vp';
const LEFT_PART_WIDTH = 'calc(66% - 16vp)';
const RIGHT_PART_WIDTH = '34%';
const RIGHT_ONLY_ARROW_WIDTH = '24vp';
const RIGHT_ONLY_IMAGE_WIDTH = '54vp';
const RIGHT_ONLY_ICON_WIDTH = '40vp';
const RIGHT_ICON_SUB_ICON_WIDTH = '80vp';
const RIGHT_ONLY_RADIO_WIDTH = '30vp';
const RIGHT_ONLY_CHECKBOX_WIDTH = '30vp';
const RIGHT_ONLY_SWITCH_WIDTH = '44vp';
const ACCESSIBILITY_LEVEL_AUTO = 'auto';
const ACCESSIBILITY_LEVEL_YES = 'yes';
const ACCESSIBILITY_LEVEL_NO = 'no';
const RESOURCE_TYPE_SYMBOL = 40000;
const ICON_SIZE_MAP = new Map([
    [IconType.BADGE, BADGE_SIZE],
    [IconType.NORMAL_ICON, SMALL_ICON_SIZE],
    [IconType.SYSTEM_ICON, SYSTEM_ICON_SIZE],
    [IconType.HEAD_SCULPTURE, HEADSCULPTURE_SIZE],
    [IconType.APP_ICON, APP_ICON_SIZE],
    [IconType.PREVIEW, PREVIEW_SIZE],
    [IconType.LONGITUDINAL, LONGITUDINAL_SIZE],
    [IconType.VERTICAL, VERTICAL_SIZE]
]);
// Does it support events such as focus, hover, press, etc. for the sub components of list
const IS_SUPPORT_SUBCOMPONENT_EVENT = LengthMetrics.resource({
    'id': -1,
    'type': 10002,
    params: ['sys.float.composeListItem_focus_dynamic_effect'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
}).value !== 1;
const RECOVER_ITEM_SCALE = 1;
const CLEAR_SHADOW = -1;
const OPERATE_ITEM_RADIUS = 50;
const OPERATE_ITEM_BACKGROUND_COLOR = '#33000000';
const DEFUALT_RADIO_CHECKBOX_BORDER_COLOR = {
    'id': -1,
    'type': 10001,
    params: ['sys.color.ohos_id_color_switch_outline_off'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
};
const OPERATE_ITEM_COLOR = '#99000000';
const TEXT_SUPPORT_MARQUEE = 1;
const IS_MARQUEE_OR_ELLIPSIS = LengthMetrics.resource({
    'id': -1,
    'type': 10002,
    params: ['sys.float.composeListItem_right_textOverflow'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
}).value;
const UNUSUAL = -1;
const FOCUSED_BG_COLOR = {
    'id': -1,
    'type': 10001,
    params: ['sys.color.composeListItem_container_focus_color'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
};
const NORMAL_BG_COLOR = {
    'id': -1,
    'type': 10001,
    params: ['sys.color.composeListItem_container_normal_color'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
};
const FOCUSED_ITEM_SCALE = LengthMetrics.resource({
    'id': -1,
    'type': 10002,
    params: ['sys.float.composeListItem_focus_magnification'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
}).value;
const FOCUSED_SHADOW = LengthMetrics.resource({
    'id': -1,
    'type': 10002,
    params: ['sys.float.composeListItem_focus_shadow_attribute'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
})
    .value;
const NORMAL_SHADOW = LengthMetrics.resource({
    'id': -1,
    'type': 10002,
    params: ['sys.float.composeListItem_normal_shadow_attribute'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
})
    .value;
const ITEM_PADDING = {
    'id': -1,
    'type': 10002,
    params: ['sys.float.composeListItem_padding'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
};
const OPERATEITEM_ARROW_MARGIN_WIDTH = LengthMetrics.resource({
    'id': -1,
    'type': 10002,
    params: ['sys.float.composeListItem_arrow_margin'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
}).value;
const APPICON_ITEMLENGTH = LengthMetrics.resource({
    'id': -1,
    'type': 10002,
    params: ['sys.float.composeListItem_AppIcon_ItemLength'],
    'bundleName': '__harDefaultBundleName__',
    'moduleName': '__harDefaultModuleName__'
}).value;

class Util {
    static isSymbolResource(resourceStr) {
        if (!Util.isResourceType(resourceStr)) {
            return false;
        }
        let resource = resourceStr;
        return resource.type === RESOURCE_TYPE_SYMBOL;
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

class ContentItemStruct extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.__iconStyle = new SynchedPropertySimpleOneWayPU(params.iconStyle, this, 'iconStyle');
        this.__icon = new SynchedPropertyObjectOneWayPU(params.icon, this, 'icon');
        this.__symbolStyle = new SynchedPropertyObjectOneWayPU(params.symbolStyle, this, 'symbolStyle');
        this.__primaryText = new SynchedPropertyObjectOneWayPU(params.primaryText, this, 'primaryText');
        this.__secondaryText = new SynchedPropertyObjectOneWayPU(params.secondaryText, this, 'secondaryText');
        this.__description = new SynchedPropertyObjectOneWayPU(params.description, this, 'description');
        this.__itemRowSpace = new ObservedPropertySimplePU(NORMAL_ITEM_ROW_SPACE, this, 'itemRowSpace');
        this.__leftWidth = new SynchedPropertySimpleOneWayPU(params.leftWidth, this, 'leftWidth');
        this.__primaryTextColor = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.ohos_id_color_text_primary'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'primaryTextColor');
        this.__secondaryTextColor = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.ohos_id_color_text_secondary'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'secondaryTextColor');
        this.__descriptionColor = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.ohos_id_color_text_secondary'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'descriptionColor');
        this.__fontSizeScale = new SynchedPropertySimpleOneWayPU(params.fontSizeScale, this, 'fontSizeScale');
        this.__parentDirection = new SynchedPropertySimpleOneWayPU(params.parentDirection, this, 'parentDirection');
        this.__itemDirection = new SynchedPropertySimpleOneWayPU(params.itemDirection, this, 'itemDirection');
        this.__isFocus = new SynchedPropertySimpleOneWayPU(params.isFocus, this, 'isFocus');
        this.__primaryTextSize = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10002,
            params: ['sys.float.ohos_id_text_size_body1'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'primaryTextSize');
        this.__primaryTextColors = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.font_primary'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'primaryTextColors');
        this.__itemHeight = new SynchedPropertyObjectOneWayPU(params.itemHeight, this, 'itemHeight');
        this.__iconColor = new ObservedPropertyObjectPU(null, this, 'iconColor');
        this.__secondaryTextColors = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.font_secondary'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'secondaryTextColors');
        this.__secondaryThirdTextSize = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10002,
            params: ['sys.float.composeListItem_left_secondary_tertiary_text_size'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'secondaryThirdTextSize');
        this.__descriptionColors = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.font_tertiary'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'descriptionColors');
        this.__isWrapText = new SynchedPropertyObjectTwoWayPU(params.isWrapText, this, 'isWrapText');
        this.__isWrapFirstText = new ObservedPropertyObjectPU(false, this, 'isWrapFirstText');
        this.__isWrapSecondText = new ObservedPropertyObjectPU(false, this, 'isWrapSecondText');
        this.__isWrapThirdText = new ObservedPropertyObjectPU(false, this, 'isWrapThirdText');
        this.setInitiallyProvidedValue(params);
        this.declareWatch('iconStyle', this.onPropChange);
        this.declareWatch('icon', this.onPropChange);
        this.declareWatch('symbolStyle', this.onPropChange);
        this.declareWatch('primaryText', this.onPropChange);
        this.declareWatch('secondaryText', this.onPropChange);
        this.declareWatch('description', this.onPropChange);
        this.declareWatch('primaryTextColor', this.onPropChange);
        this.declareWatch('secondaryTextColor', this.onPropChange);
        this.declareWatch('descriptionColor', this.onPropChange);
        this.declareWatch('isFocus', this.onPropChange);
        this.declareWatch('isWrapFirstText', this.onWrapChange);
        this.declareWatch('isWrapSecondText', this.onWrapChange);
        this.declareWatch('isWrapThirdText', this.onWrapChange);
        this.finalizeConstruction();
    }

    setInitiallyProvidedValue(params) {
        if (params.iconStyle === undefined) {
            this.__iconStyle.set(null);
        }
        if (params.icon === undefined) {
            this.__icon.set(null);
        }
        if (params.symbolStyle === undefined) {
            this.__symbolStyle.set(null);
        }
        if (params.primaryText === undefined) {
            this.__primaryText.set(null);
        }
        if (params.secondaryText === undefined) {
            this.__secondaryText.set(null);
        }
        if (params.description === undefined) {
            this.__description.set(null);
        }
        if (params.itemRowSpace !== undefined) {
            this.itemRowSpace = params.itemRowSpace;
        }
        if (params.leftWidth === undefined) {
            this.__leftWidth.set(LEFT_PART_WIDTH);
        }
        if (params.primaryTextColor !== undefined) {
            this.primaryTextColor = params.primaryTextColor;
        }
        if (params.secondaryTextColor !== undefined) {
            this.secondaryTextColor = params.secondaryTextColor;
        }
        if (params.descriptionColor !== undefined) {
            this.descriptionColor = params.descriptionColor;
        }
        if (params.isFocus === undefined) {
            this.__isFocus.set(false);
        }
        if (params.primaryTextSize !== undefined) {
            this.primaryTextSize = params.primaryTextSize;
        }
        if (params.primaryTextColors !== undefined) {
            this.primaryTextColors = params.primaryTextColors;
        }
        if (params.itemHeight === undefined) {
            this.__itemHeight.set(null);
        }
        if (params.iconColor !== undefined) {
            this.iconColor = params.iconColor;
        }
        if (params.secondaryTextColors !== undefined) {
            this.secondaryTextColors = params.secondaryTextColors;
        }
        if (params.secondaryThirdTextSize !== undefined) {
            this.secondaryThirdTextSize = params.secondaryThirdTextSize;
        }
        if (params.descriptionColors !== undefined) {
            this.descriptionColors = params.descriptionColors;
        }
        if (params.isWrapFirstText !== undefined) {
            this.isWrapFirstText = params.isWrapFirstText;
        }
        if (params.isWrapSecondText !== undefined) {
            this.isWrapSecondText = params.isWrapSecondText;
        }
        if (params.isWrapThirdText !== undefined) {
            this.isWrapThirdText = params.isWrapThirdText;
        }
    }

    updateStateVars(params) {
        this.__iconStyle.reset(params.iconStyle);
        this.__icon.reset(params.icon);
        this.__symbolStyle.reset(params.symbolStyle);
        this.__primaryText.reset(params.primaryText);
        this.__secondaryText.reset(params.secondaryText);
        this.__description.reset(params.description);
        this.__leftWidth.reset(params.leftWidth);
        this.__fontSizeScale.reset(params.fontSizeScale);
        this.__parentDirection.reset(params.parentDirection);
        this.__itemDirection.reset(params.itemDirection);
        this.__isFocus.reset(params.isFocus);
        this.__itemHeight.reset(params.itemHeight);
    }

    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__iconStyle.purgeDependencyOnElmtId(rmElmtId);
        this.__icon.purgeDependencyOnElmtId(rmElmtId);
        this.__symbolStyle.purgeDependencyOnElmtId(rmElmtId);
        this.__primaryText.purgeDependencyOnElmtId(rmElmtId);
        this.__secondaryText.purgeDependencyOnElmtId(rmElmtId);
        this.__description.purgeDependencyOnElmtId(rmElmtId);
        this.__itemRowSpace.purgeDependencyOnElmtId(rmElmtId);
        this.__leftWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__primaryTextColor.purgeDependencyOnElmtId(rmElmtId);
        this.__secondaryTextColor.purgeDependencyOnElmtId(rmElmtId);
        this.__descriptionColor.purgeDependencyOnElmtId(rmElmtId);
        this.__fontSizeScale.purgeDependencyOnElmtId(rmElmtId);
        this.__parentDirection.purgeDependencyOnElmtId(rmElmtId);
        this.__itemDirection.purgeDependencyOnElmtId(rmElmtId);
        this.__isFocus.purgeDependencyOnElmtId(rmElmtId);
        this.__primaryTextSize.purgeDependencyOnElmtId(rmElmtId);
        this.__primaryTextColors.purgeDependencyOnElmtId(rmElmtId);
        this.__itemHeight.purgeDependencyOnElmtId(rmElmtId);
        this.__iconColor.purgeDependencyOnElmtId(rmElmtId);
        this.__secondaryTextColors.purgeDependencyOnElmtId(rmElmtId);
        this.__secondaryThirdTextSize.purgeDependencyOnElmtId(rmElmtId);
        this.__descriptionColors.purgeDependencyOnElmtId(rmElmtId);
        this.__isWrapText.purgeDependencyOnElmtId(rmElmtId);
        this.__isWrapFirstText.purgeDependencyOnElmtId(rmElmtId);
        this.__isWrapSecondText.purgeDependencyOnElmtId(rmElmtId);
        this.__isWrapThirdText.purgeDependencyOnElmtId(rmElmtId);
    }

    aboutToBeDeleted() {
        this.__iconStyle.aboutToBeDeleted();
        this.__icon.aboutToBeDeleted();
        this.__symbolStyle.aboutToBeDeleted();
        this.__primaryText.aboutToBeDeleted();
        this.__secondaryText.aboutToBeDeleted();
        this.__description.aboutToBeDeleted();
        this.__itemRowSpace.aboutToBeDeleted();
        this.__leftWidth.aboutToBeDeleted();
        this.__primaryTextColor.aboutToBeDeleted();
        this.__secondaryTextColor.aboutToBeDeleted();
        this.__descriptionColor.aboutToBeDeleted();
        this.__fontSizeScale.aboutToBeDeleted();
        this.__parentDirection.aboutToBeDeleted();
        this.__itemDirection.aboutToBeDeleted();
        this.__isFocus.aboutToBeDeleted();
        this.__primaryTextSize.aboutToBeDeleted();
        this.__primaryTextColors.aboutToBeDeleted();
        this.__itemHeight.aboutToBeDeleted();
        this.__iconColor.aboutToBeDeleted();
        this.__secondaryTextColors.aboutToBeDeleted();
        this.__secondaryThirdTextSize.aboutToBeDeleted();
        this.__descriptionColors.aboutToBeDeleted();
        this.__isWrapText.aboutToBeDeleted();
        this.__isWrapFirstText.aboutToBeDeleted();
        this.__isWrapSecondText.aboutToBeDeleted();
        this.__isWrapThirdText.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    get iconStyle() {
        return this.__iconStyle.get();
    }

    set iconStyle(newValue) {
        this.__iconStyle.set(newValue);
    }

    get icon() {
        return this.__icon.get();
    }

    set icon(newValue) {
        this.__icon.set(newValue);
    }

    get symbolStyle() {
        return this.__symbolStyle.get();
    }

    set symbolStyle(newValue) {
        this.__symbolStyle.set(newValue);
    }

    get primaryText() {
        return this.__primaryText.get();
    }

    set primaryText(newValue) {
        this.__primaryText.set(newValue);
    }

    get secondaryText() {
        return this.__secondaryText.get();
    }

    set secondaryText(newValue) {
        this.__secondaryText.set(newValue);
    }

    get description() {
        return this.__description.get();
    }

    set description(newValue) {
        this.__description.set(newValue);
    }

    get itemRowSpace() {
        return this.__itemRowSpace.get();
    }

    set itemRowSpace(newValue) {
        this.__itemRowSpace.set(newValue);
    }

    get leftWidth() {
        return this.__leftWidth.get();
    }

    set leftWidth(newValue) {
        this.__leftWidth.set(newValue);
    }

    get primaryTextColor() {
        return this.__primaryTextColor.get();
    }

    set primaryTextColor(newValue) {
        this.__primaryTextColor.set(newValue);
    }

    get secondaryTextColor() {
        return this.__secondaryTextColor.get();
    }

    set secondaryTextColor(newValue) {
        this.__secondaryTextColor.set(newValue);
    }

    get descriptionColor() {
        return this.__descriptionColor.get();
    }

    set descriptionColor(newValue) {
        this.__descriptionColor.set(newValue);
    }

    get fontSizeScale() {
        return this.__fontSizeScale.get();
    }

    set fontSizeScale(newValue) {
        this.__fontSizeScale.set(newValue);
    }

    get parentDirection() {
        return this.__parentDirection.get();
    }

    set parentDirection(newValue) {
        this.__parentDirection.set(newValue);
    }

    get itemDirection() {
        return this.__itemDirection.get();
    }

    set itemDirection(newValue) {
        this.__itemDirection.set(newValue);
    }

    get isFocus() {
        return this.__isFocus.get();
    }

    set isFocus(newValue) {
        this.__isFocus.set(newValue);
    }

    get primaryTextSize() {
        return this.__primaryTextSize.get();
    }

    set primaryTextSize(newValue) {
        this.__primaryTextSize.set(newValue);
    }

    get primaryTextColors() {
        return this.__primaryTextColors.get();
    }

    set primaryTextColors(newValue) {
        this.__primaryTextColors.set(newValue);
    }

    get itemHeight() {
        return this.__itemHeight.get();
    }

    set itemHeight(newValue) {
        this.__itemHeight.set(newValue);
    }

    get iconColor() {
        return this.__iconColor.get();
    }

    set iconColor(newValue) {
        this.__iconColor.set(newValue);
    }

    get secondaryTextColors() {
        return this.__secondaryTextColors.get();
    }

    set secondaryTextColors(newValue) {
        this.__secondaryTextColors.set(newValue);
    }

    get secondaryThirdTextSize() {
        return this.__secondaryThirdTextSize.get();
    }

    set secondaryThirdTextSize(newValue) {
        this.__secondaryThirdTextSize.set(newValue);
    }

    get descriptionColors() {
        return this.__descriptionColors.get();
    }

    set descriptionColors(newValue) {
        this.__descriptionColors.set(newValue);
    }

    get isWrapText() {
        return this.__isWrapText.get();
    }

    set isWrapText(newValue) {
        this.__isWrapText.set(newValue);
    }

    get isWrapFirstText() {
        return this.__isWrapFirstText.get();
    }

    set isWrapFirstText(newValue) {
        this.__isWrapFirstText.set(newValue);
    }

    get isWrapSecondText() {
        return this.__isWrapSecondText.get();
    }

    set isWrapSecondText(newValue) {
        this.__isWrapSecondText.set(newValue);
    }

    get isWrapThirdText() {
        return this.__isWrapThirdText.get();
    }

    set isWrapThirdText(newValue) {
        this.__isWrapThirdText.set(newValue);
    }

    onWillApplyTheme(theme) {
        this.primaryTextColor = theme.colors.fontPrimary;
        this.secondaryTextColor = theme.colors.fontSecondary;
        this.descriptionColor = theme.colors.fontTertiary;
    }

    onPropChange() {
        if (this.icon == null && this.symbolStyle == null && this.iconStyle == null) {
            this.itemRowSpace = SPECIAL_ITEM_ROW_SPACE;
        } else {
            this.itemRowSpace = NORMAL_ITEM_ROW_SPACE;
        }
        if (!IS_SUPPORT_SUBCOMPONENT_EVENT && this.isFocus) {
            this.primaryTextColors = {
                'id': -1,
                'type': 10001,
                params: ['sys.color.composeListItem_left_text_focus_color'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            };
            this.secondaryTextColors = {
                'id': -1,
                'type': 10001,
                params: ['sys.color.composeListItem_left_secondary_text_focus_color'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            };
            this.descriptionColors = {
                'id': -1,
                'type': 10001,
                params: ['sys.color.composeListItem_left_secondary_text_focus_color'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            };
        } else {
            this.primaryTextColors = this.primaryTextColor;
            this.secondaryTextColors = this.secondaryTextColor;
            this.descriptionColors = this.descriptionColor;
        }
    }

    onWrapChange() {
        this.isWrapText = this.isWrapFirstText || this.isWrapSecondText || this.isWrapThirdText;
    }

    getContentItemIconFillColor() {
        switch (this.iconStyle) {
            case IconType.BADGE:
                return {
                    'id': -1,
                    'type': 10001,
                    params: ['sys.color.composeListItem_badge_color'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                };
            case IconType.SYSTEM_ICON:
                return {
                    'id': -1,
                    'type': 10001,
                    params: ['sys.color.composeListItem_icon_normal_color'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                };
            default:
                return {
                    'id': -1,
                    'type': 10001,
                    params: ['sys.color.ohos_id_color_secondary'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                };
        }
    }

    judgeIsWrap(text, sizeResource, newHeight) {
        let singleRowHeight = this.getSingleRowTextHeight(text, sizeResource);
        return newHeight > singleRowHeight;
    }

    getSingleRowTextHeight(text, sizeResource) {
        if (text && sizeResource) {
            let singleRowHeight = px2vp(measure.measureTextSize({
                textContent: text,
                fontSize: sizeResource,
                maxLines: TEXT_MAX_LINE
            }).height);
            return singleRowHeight;
        }
        return 0;
    }

    aboutToAppear() {
        this.onPropChange();
    }

    createIcon(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.iconStyle != null && ICON_SIZE_MAP.has(this.iconStyle)) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.symbolStyle != null) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create();
                                    SymbolGlyph.fontColor([this.getContentItemIconFillColor()]);
                                    SymbolGlyph.attributeModifier.bind(this)(ObservedObject.GetRawObject(this.symbolStyle));
                                    SymbolGlyph.fontSize(`${ICON_SIZE_MAP.get(this.iconStyle)}vp`);
                                    SymbolGlyph.effectStrategy(SymbolEffectStrategy.NONE);
                                    SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                                    SymbolGlyph.borderRadius({
                                        'id': -1,
                                        'type': 10002,
                                        params: ['sys.float.composeListItem_Image_Radius'],
                                        'bundleName': '__harDefaultBundleName__',
                                        'moduleName': '__harDefaultModuleName__'
                                    });
                                    SymbolGlyph.focusable(false);
                                    SymbolGlyph.draggable(false);
                                    SymbolGlyph.flexShrink(0);
                                }, SymbolGlyph);
                            });
                        } else if (this.icon != null) {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    If.create();
                                    if (Util.isSymbolResource(ObservedObject.GetRawObject(this.icon))) {
                                        this.ifElseBranchUpdateFunction(0, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                SymbolGlyph.create(this.icon);
                                                SymbolGlyph.fontSize(`${ICON_SIZE_MAP.get(this.iconStyle)}vp`);
                                                SymbolGlyph.fontColor([this.getContentItemIconFillColor()]);
                                                SymbolGlyph.borderRadius({
                                                    'id': -1,
                                                    'type': 10002,
                                                    params: ['sys.float.composeListItem_Image_Radius'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__'
                                                });
                                                SymbolGlyph.focusable(false);
                                                SymbolGlyph.draggable(false);
                                                SymbolGlyph.flexShrink(0);
                                            }, SymbolGlyph);
                                        });
                                    } else {
                                        this.ifElseBranchUpdateFunction(1, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                If.create();
                                                if (this.iconStyle <= IconType.PREVIEW) {
                                                    this.ifElseBranchUpdateFunction(0, () => {
                                                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                            Image.create(this.icon);
                                                            Image.objectFit(ImageFit.Contain);
                                                            Image.width(ICON_SIZE_MAP.get(this.iconStyle));
                                                            Image.height(ICON_SIZE_MAP.get(this.iconStyle));
                                                            Image.borderRadius({
                                                                'id': -1,
                                                                'type': 10002,
                                                                params: ['sys.float.composeListItem_Image_Radius'],
                                                                'bundleName': '__harDefaultBundleName__',
                                                                'moduleName': '__harDefaultModuleName__'
                                                            });
                                                            Image.focusable(false);
                                                            Image.draggable(false);
                                                            Image.fillColor(this.getContentItemIconFillColor());
                                                            Image.flexShrink(0);
                                                        }, Image);
                                                    });
                                                } else {
                                                    this.ifElseBranchUpdateFunction(1, () => {
                                                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                            Image.create(this.icon);
                                                            Image.objectFit(ImageFit.Contain);
                                                            Image.constraintSize({
                                                                minWidth: SPECIAL_ICON_SIZE,
                                                                maxWidth: ICON_SIZE_MAP.get(this.iconStyle),
                                                                minHeight: SPECIAL_ICON_SIZE,
                                                                maxHeight: ICON_SIZE_MAP.get(this.iconStyle)
                                                            });
                                                            Image.borderRadius({
                                                                'id': -1,
                                                                'type': 10002,
                                                                params: ['sys.float.composeListItem_Image_Radius'],
                                                                'bundleName': '__harDefaultBundleName__',
                                                                'moduleName': '__harDefaultModuleName__'
                                                            });
                                                            Image.focusable(false);
                                                            Image.draggable(false);
                                                            Image.fillColor(this.getContentItemIconFillColor());
                                                            Image.flexShrink(0);
                                                        }, Image);
                                                    });
                                                }
                                            }, If);
                                            If.pop();
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
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
    }

    createText(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create({ space: TEXT_COLUMN_SPACE });
            Column.flexShrink(1);
            Column.margin(this.fontSizeScale >= FontSizeScaleLevel.LEVEL1 ? undefined : {
                top: TEXT_SAFE_MARGIN,
                bottom: TEXT_SAFE_MARGIN
            });
            Column.alignItems(HorizontalAlign.Start);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(this.primaryText);
            Text.fontSize(ObservedObject.GetRawObject(this.primaryTextSize));
            Text.fontColor(ObservedObject.GetRawObject(this.primaryTextColors));
            Text.textOverflow({
                overflow: IS_MARQUEE_OR_ELLIPSIS === TEXT_SUPPORT_MARQUEE ? TextOverflow.None :
                TextOverflow.Ellipsis
            });
            Text.fontWeight(FontWeight.Medium);
            Text.focusable(true);
            Text.draggable(false);
            Text.onSizeChange((oldValue, newValue) => {
                if (!IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.isWrapFirstText = this.judgeIsWrap(ObservedObject.GetRawObject(this.primaryText),
                        ObservedObject.GetRawObject(this.primaryTextSize), newValue.height);
                }
            });
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.secondaryText != null) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(this.secondaryText);
                        Text.fontSize(ObservedObject.GetRawObject(this.secondaryThirdTextSize));
                        Text.fontColor(ObservedObject.GetRawObject(this.secondaryTextColors));
                        Text.textOverflow({
                            overflow: IS_MARQUEE_OR_ELLIPSIS === TEXT_SUPPORT_MARQUEE ? TextOverflow.None :
                            TextOverflow.Ellipsis
                        });
                        Text.draggable(false);
                        Text.onSizeChange((oldValue, newValue) => {
                            if (!IS_SUPPORT_SUBCOMPONENT_EVENT) {
                                this.isWrapSecondText =
                                    this.judgeIsWrap(ObservedObject.GetRawObject(this.secondaryText),
                                        ObservedObject.GetRawObject(this.secondaryThirdTextSize), newValue.height);
                            }
                        });
                    }, Text);
                    Text.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.description != null) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(this.description);
                        Text.fontSize(ObservedObject.GetRawObject(this.secondaryThirdTextSize));
                        Text.fontColor(ObservedObject.GetRawObject(this.descriptionColors));
                        Text.textOverflow({
                            overflow: IS_MARQUEE_OR_ELLIPSIS === TEXT_SUPPORT_MARQUEE ? TextOverflow.None :
                            TextOverflow.Ellipsis
                        });
                        Text.draggable(false);
                        Text.onSizeChange((oldValue, newValue) => {
                            if (!IS_SUPPORT_SUBCOMPONENT_EVENT) {
                                this.isWrapThirdText = this.judgeIsWrap(ObservedObject.GetRawObject(this.description),
                                    ObservedObject.GetRawObject(this.secondaryThirdTextSize), newValue.height);
                            }
                        });
                    }, Text);
                    Text.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Column.pop();
    }

    isColumnDirection() {
        return this.itemDirection === FlexDirection.Column;
    }

    isParentColumnDirection() {
        return this.parentDirection === FlexDirection.Column;
    }

    getItemSpace() {
        if (this.isColumnDirection()) {
            return LengthMetrics.resource({
                'id': -1,
                'type': 10002,
                params: ['sys.float.padding_level1'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            });
        }
        return LengthMetrics.vp(this.itemRowSpace);
    }

    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Flex.create({
                space: { main: this.getItemSpace() },
                direction: this.itemDirection,
                justifyContent: FlexAlign.Start,
                alignItems: this.isColumnDirection() ? ItemAlign.Start : ItemAlign.Center,
            });
            Flex.height(this.itemDirection === FlexDirection.Column ? 'auto' : undefined);
            Flex.margin({
                end: this.isParentColumnDirection() ?
                LengthMetrics.vp(0) :
                LengthMetrics.vp(16)
            });
            Flex.padding({ start: LengthMetrics.vp(LISTITEM_PADDING) });
            Flex.flexShrink(this.isParentColumnDirection() ? 0 : 1);
        }, Flex);
        this.createIcon.bind(this)();
        this.createText.bind(this)();
        Flex.pop();
    }

    rerender() {
        this.updateDirtyElements();
    }
}

class CreateIconParam {
}

class OperateItemStruct extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.__arrow = new SynchedPropertyObjectOneWayPU(params.arrow, this, 'arrow');
        this.__icon = new SynchedPropertyObjectOneWayPU(params.icon, this, 'icon');
        this.__subIcon = new SynchedPropertyObjectOneWayPU(params.subIcon, this, 'subIcon');
        this.__button = new SynchedPropertyObjectOneWayPU(params.button, this, 'button');
        this.__switch = new SynchedPropertyObjectOneWayPU(params.switch, this, 'switch');
        this.__checkBox = new SynchedPropertyObjectOneWayPU(params.checkBox, this, 'checkBox');
        this.__radio = new SynchedPropertyObjectOneWayPU(params.radio, this, 'radio');
        this.__image = new SynchedPropertyObjectOneWayPU(params.image, this, 'image');
        this.__symbolStyle = new SynchedPropertyObjectOneWayPU(params.symbolStyle, this, 'symbolStyle');
        this.__text = new SynchedPropertyObjectOneWayPU(params.text, this, 'text');
        this.__switchState = new ObservedPropertySimplePU(false, this, 'switchState');
        this.__radioState = new ObservedPropertySimplePU(false, this, 'radioState');
        this.__checkBoxState = new ObservedPropertySimplePU(false, this, 'checkBoxState');
        this.__rightWidth = new SynchedPropertySimpleOneWayPU(params.rightWidth, this, 'rightWidth');
        this.__secondaryTextColor = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.ohos_id_color_text_secondary'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'secondaryTextColor');
        this.__hoveringColor = new ObservedPropertyObjectPU('#0d000000', this, 'hoveringColor');
        this.__activedColor = new ObservedPropertyObjectPU('#1a0a59f7', this, 'activedColor');
        this.__parentCanFocus = new SynchedPropertySimpleTwoWayPU(params.parentCanFocus, this, 'parentCanFocus');
        this.__parentCanTouch = new SynchedPropertySimpleTwoWayPU(params.parentCanTouch, this, 'parentCanTouch');
        this.__parentIsHover = new SynchedPropertySimpleTwoWayPU(params.parentIsHover, this, 'parentIsHover');
        this.__parentCanHover = new SynchedPropertySimpleTwoWayPU(params.parentCanHover, this, 'parentCanHover');
        this.__parentIsActive = new SynchedPropertySimpleTwoWayPU(params.parentIsActive, this, 'parentIsActive');
        this.__parentFrontColor = new SynchedPropertyObjectTwoWayPU(params.parentFrontColor, this, 'parentFrontColor');
        this.__parentDirection = new SynchedPropertySimpleTwoWayPU(params.parentDirection, this, 'parentDirection');
        this.__rowSpace = new ObservedPropertySimplePU(DEFAULT_ROW_SPACE, this, 'rowSpace');
        this.__isFocus = new SynchedPropertySimpleTwoWayPU(params.isFocus, this, 'isFocus');
        this.__secondaryTextSize = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10002,
            params: ['sys.float.ohos_id_text_size_body2'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'secondaryTextSize');
        this.__secondaryTextColors = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.font_secondary'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'secondaryTextColors');
        this.__iconColor = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.composeListItem_right_icon_normal_color'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'iconColor');
        this.__isChecked = new SynchedPropertySimpleTwoWayPU(params.isChecked, this, 'isChecked');
        this.setInitiallyProvidedValue(params);
        this.declareWatch('arrow', this.onPropChange);
        this.declareWatch('icon', this.onPropChange);
        this.declareWatch('subIcon', this.onPropChange);
        this.declareWatch('button', this.onPropChange);
        this.declareWatch('switch', this.onPropChange);
        this.declareWatch('checkBox', this.onPropChange);
        this.declareWatch('radio', this.onPropChange);
        this.declareWatch('image', this.onPropChange);
        this.declareWatch('symbolStyle', this.onPropChange);
        this.declareWatch('text', this.onPropChange);
        this.declareWatch('secondaryTextColor', this.onFocusChange);
        this.declareWatch('isFocus', this.onFocusChange);
        this.declareWatch('isChecked', this.onPropChange);
        this.finalizeConstruction();
    }

    setInitiallyProvidedValue(params) {
        if (params.arrow === undefined) {
            this.__arrow.set(null);
        }
        if (params.icon === undefined) {
            this.__icon.set(null);
        }
        if (params.subIcon === undefined) {
            this.__subIcon.set(null);
        }
        if (params.button === undefined) {
            this.__button.set(null);
        }
        if (params.switch === undefined) {
            this.__switch.set(null);
        }
        if (params.checkBox === undefined) {
            this.__checkBox.set(null);
        }
        if (params.radio === undefined) {
            this.__radio.set(null);
        }
        if (params.image === undefined) {
            this.__image.set(null);
        }
        if (params.symbolStyle === undefined) {
            this.__symbolStyle.set(null);
        }
        if (params.text === undefined) {
            this.__text.set(null);
        }
        if (params.switchState !== undefined) {
            this.switchState = params.switchState;
        }
        if (params.radioState !== undefined) {
            this.radioState = params.radioState;
        }
        if (params.checkBoxState !== undefined) {
            this.checkBoxState = params.checkBoxState;
        }
        if (params.rightWidth === undefined) {
            this.__rightWidth.set(RIGHT_PART_WIDTH);
        }
        if (params.secondaryTextColor !== undefined) {
            this.secondaryTextColor = params.secondaryTextColor;
        }
        if (params.hoveringColor !== undefined) {
            this.hoveringColor = params.hoveringColor;
        }
        if (params.activedColor !== undefined) {
            this.activedColor = params.activedColor;
        }
        if (params.rowSpace !== undefined) {
            this.rowSpace = params.rowSpace;
        }
        if (params.secondaryTextSize !== undefined) {
            this.secondaryTextSize = params.secondaryTextSize;
        }
        if (params.secondaryTextColors !== undefined) {
            this.secondaryTextColors = params.secondaryTextColors;
        }
        if (params.iconColor !== undefined) {
            this.iconColor = params.iconColor;
        }
    }

    updateStateVars(params) {
        this.__arrow.reset(params.arrow);
        this.__icon.reset(params.icon);
        this.__subIcon.reset(params.subIcon);
        this.__button.reset(params.button);
        this.__switch.reset(params.switch);
        this.__checkBox.reset(params.checkBox);
        this.__radio.reset(params.radio);
        this.__image.reset(params.image);
        this.__symbolStyle.reset(params.symbolStyle);
        this.__text.reset(params.text);
        this.__rightWidth.reset(params.rightWidth);
    }

    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__arrow.purgeDependencyOnElmtId(rmElmtId);
        this.__icon.purgeDependencyOnElmtId(rmElmtId);
        this.__subIcon.purgeDependencyOnElmtId(rmElmtId);
        this.__button.purgeDependencyOnElmtId(rmElmtId);
        this.__switch.purgeDependencyOnElmtId(rmElmtId);
        this.__checkBox.purgeDependencyOnElmtId(rmElmtId);
        this.__radio.purgeDependencyOnElmtId(rmElmtId);
        this.__image.purgeDependencyOnElmtId(rmElmtId);
        this.__symbolStyle.purgeDependencyOnElmtId(rmElmtId);
        this.__text.purgeDependencyOnElmtId(rmElmtId);
        this.__switchState.purgeDependencyOnElmtId(rmElmtId);
        this.__radioState.purgeDependencyOnElmtId(rmElmtId);
        this.__checkBoxState.purgeDependencyOnElmtId(rmElmtId);
        this.__rightWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__secondaryTextColor.purgeDependencyOnElmtId(rmElmtId);
        this.__hoveringColor.purgeDependencyOnElmtId(rmElmtId);
        this.__activedColor.purgeDependencyOnElmtId(rmElmtId);
        this.__parentCanFocus.purgeDependencyOnElmtId(rmElmtId);
        this.__parentCanTouch.purgeDependencyOnElmtId(rmElmtId);
        this.__parentIsHover.purgeDependencyOnElmtId(rmElmtId);
        this.__parentCanHover.purgeDependencyOnElmtId(rmElmtId);
        this.__parentIsActive.purgeDependencyOnElmtId(rmElmtId);
        this.__parentFrontColor.purgeDependencyOnElmtId(rmElmtId);
        this.__parentDirection.purgeDependencyOnElmtId(rmElmtId);
        this.__rowSpace.purgeDependencyOnElmtId(rmElmtId);
        this.__isFocus.purgeDependencyOnElmtId(rmElmtId);
        this.__secondaryTextSize.purgeDependencyOnElmtId(rmElmtId);
        this.__secondaryTextColors.purgeDependencyOnElmtId(rmElmtId);
        this.__iconColor.purgeDependencyOnElmtId(rmElmtId);
        this.__isChecked.purgeDependencyOnElmtId(rmElmtId);
    }

    aboutToBeDeleted() {
        this.__arrow.aboutToBeDeleted();
        this.__icon.aboutToBeDeleted();
        this.__subIcon.aboutToBeDeleted();
        this.__button.aboutToBeDeleted();
        this.__switch.aboutToBeDeleted();
        this.__checkBox.aboutToBeDeleted();
        this.__radio.aboutToBeDeleted();
        this.__image.aboutToBeDeleted();
        this.__symbolStyle.aboutToBeDeleted();
        this.__text.aboutToBeDeleted();
        this.__switchState.aboutToBeDeleted();
        this.__radioState.aboutToBeDeleted();
        this.__checkBoxState.aboutToBeDeleted();
        this.__rightWidth.aboutToBeDeleted();
        this.__secondaryTextColor.aboutToBeDeleted();
        this.__hoveringColor.aboutToBeDeleted();
        this.__activedColor.aboutToBeDeleted();
        this.__parentCanFocus.aboutToBeDeleted();
        this.__parentCanTouch.aboutToBeDeleted();
        this.__parentIsHover.aboutToBeDeleted();
        this.__parentCanHover.aboutToBeDeleted();
        this.__parentIsActive.aboutToBeDeleted();
        this.__parentFrontColor.aboutToBeDeleted();
        this.__parentDirection.aboutToBeDeleted();
        this.__rowSpace.aboutToBeDeleted();
        this.__isFocus.aboutToBeDeleted();
        this.__secondaryTextSize.aboutToBeDeleted();
        this.__secondaryTextColors.aboutToBeDeleted();
        this.__iconColor.aboutToBeDeleted();
        this.__isChecked.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    get arrow() {
        return this.__arrow.get();
    }

    set arrow(newValue) {
        this.__arrow.set(newValue);
    }

    get icon() {
        return this.__icon.get();
    }

    set icon(newValue) {
        this.__icon.set(newValue);
    }

    get subIcon() {
        return this.__subIcon.get();
    }

    set subIcon(newValue) {
        this.__subIcon.set(newValue);
    }

    get button() {
        return this.__button.get();
    }

    set button(newValue) {
        this.__button.set(newValue);
    }

    get switch() {
        return this.__switch.get();
    }

    set switch(newValue) {
        this.__switch.set(newValue);
    }

    get checkBox() {
        return this.__checkBox.get();
    }

    set checkBox(newValue) {
        this.__checkBox.set(newValue);
    }

    get radio() {
        return this.__radio.get();
    }

    set radio(newValue) {
        this.__radio.set(newValue);
    }

    get image() {
        return this.__image.get();
    }

    set image(newValue) {
        this.__image.set(newValue);
    }

    get symbolStyle() {
        return this.__symbolStyle.get();
    }

    set symbolStyle(newValue) {
        this.__symbolStyle.set(newValue);
    }

    get text() {
        return this.__text.get();
    }

    set text(newValue) {
        this.__text.set(newValue);
    }

    get switchState() {
        return this.__switchState.get();
    }

    set switchState(newValue) {
        this.__switchState.set(newValue);
    }

    get radioState() {
        return this.__radioState.get();
    }

    set radioState(newValue) {
        this.__radioState.set(newValue);
    }

    get checkBoxState() {
        return this.__checkBoxState.get();
    }

    set checkBoxState(newValue) {
        this.__checkBoxState.set(newValue);
    }

    get rightWidth() {
        return this.__rightWidth.get();
    }

    set rightWidth(newValue) {
        this.__rightWidth.set(newValue);
    }

    get secondaryTextColor() {
        return this.__secondaryTextColor.get();
    }

    set secondaryTextColor(newValue) {
        this.__secondaryTextColor.set(newValue);
    }

    get hoveringColor() {
        return this.__hoveringColor.get();
    }

    set hoveringColor(newValue) {
        this.__hoveringColor.set(newValue);
    }

    get activedColor() {
        return this.__activedColor.get();
    }

    set activedColor(newValue) {
        this.__activedColor.set(newValue);
    }

    get parentCanFocus() {
        return this.__parentCanFocus.get();
    }

    set parentCanFocus(newValue) {
        this.__parentCanFocus.set(newValue);
    }

    get parentCanTouch() {
        return this.__parentCanTouch.get();
    }

    set parentCanTouch(newValue) {
        this.__parentCanTouch.set(newValue);
    }

    get parentIsHover() {
        return this.__parentIsHover.get();
    }

    set parentIsHover(newValue) {
        this.__parentIsHover.set(newValue);
    }

    get parentCanHover() {
        return this.__parentCanHover.get();
    }

    set parentCanHover(newValue) {
        this.__parentCanHover.set(newValue);
    }

    get parentIsActive() {
        return this.__parentIsActive.get();
    }

    set parentIsActive(newValue) {
        this.__parentIsActive.set(newValue);
    }

    get parentFrontColor() {
        return this.__parentFrontColor.get();
    }

    set parentFrontColor(newValue) {
        this.__parentFrontColor.set(newValue);
    }

    get parentDirection() {
        return this.__parentDirection.get();
    }

    set parentDirection(newValue) {
        this.__parentDirection.set(newValue);
    }

    get rowSpace() {
        return this.__rowSpace.get();
    }

    set rowSpace(newValue) {
        this.__rowSpace.set(newValue);
    }

    get isFocus() {
        return this.__isFocus.get();
    }

    set isFocus(newValue) {
        this.__isFocus.set(newValue);
    }

    get secondaryTextSize() {
        return this.__secondaryTextSize.get();
    }

    set secondaryTextSize(newValue) {
        this.__secondaryTextSize.set(newValue);
    }

    get secondaryTextColors() {
        return this.__secondaryTextColors.get();
    }

    set secondaryTextColors(newValue) {
        this.__secondaryTextColors.set(newValue);
    }

    get iconColor() {
        return this.__iconColor.get();
    }

    set iconColor(newValue) {
        this.__iconColor.set(newValue);
    }

    get isChecked() {
        return this.__isChecked.get();
    }

    set isChecked(newValue) {
        this.__isChecked.set(newValue);
    }

    onWillApplyTheme(theme) {
        this.secondaryTextColor = theme.colors.fontSecondary;
        this.hoveringColor = theme.colors.interactiveHover;
        this.activedColor = theme.colors.interactiveActive;
    }

    onFocusChange() {
        if (!IS_SUPPORT_SUBCOMPONENT_EVENT && this.isFocus) {
            this.secondaryTextColors = {
                'id': -1,
                'type': 10001,
                params: ['sys.color.composeListItem_right_text_focus_color'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            };
        } else {
            this.secondaryTextColors = this.secondaryTextColor;
        }
        this.iconColor = this.isFocus ? {
            'id': -1,
            'type': 10001,
            params: ['sys.color.composeListItem_right_icon_focus_color'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        } : {
            'id': -1,
            'type': 10001,
            params: ['sys.color.composeListItem_right_icon_normal_color'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        };
    }

    onPropChange() {
        if (this.switch != null) {
            this.switchState = IS_SUPPORT_SUBCOMPONENT_EVENT ? this.switch.isCheck : this.isChecked;
        }
        if (this.radio != null) {
            this.radioState = IS_SUPPORT_SUBCOMPONENT_EVENT ? this.radio.isCheck : this.isChecked;
        }
        if (this.checkBox != null) {
            this.checkBoxState = IS_SUPPORT_SUBCOMPONENT_EVENT ? this.checkBox.isCheck : this.isChecked;
        }
        if ((this.button == null && this.image == null && this.symbolStyle == null && this.text != null) &&
            ((this.icon != null) || (this.icon == null && this.arrow != null))) {
            this.rowSpace = SPECICAL_ROW_SPACE;
        } else {
            this.rowSpace = DEFAULT_ROW_SPACE;
        }
    }

    getUnselectedColor() {
        if (IS_SUPPORT_SUBCOMPONENT_EVENT) {
            return DEFUALT_RADIO_CHECKBOX_BORDER_COLOR;
        }
        return this.isFocus ? OPERATE_ITEM_COLOR : DEFUALT_RADIO_CHECKBOX_BORDER_COLOR;
    }

    aboutToAppear() {
        if (this.switch !== null) {
            this.isChecked = this.switch.isCheck;
        }
        if (this.radio !== null) {
            this.isChecked = this.radio.isCheck;
        }
        if (this.checkBox !== null) {
            this.isChecked = this.checkBox.isCheck;
        }
        this.onPropChange();
        this.onFocusChange();
    }

    createButton(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Button.createWithChild();
            Button.padding({ top: 0, bottom: 0 });
            Button.margin({ end: LengthMetrics.vp(LISTITEM_PADDING) });
            Button.hitTestBehavior(IS_SUPPORT_SUBCOMPONENT_EVENT ? HitTestMode.Block : HitTestMode.None);
            Button.fontSize({
                'id': -1,
                'type': 10002,
                params: ['sys.float.ohos_id_text_size_button3'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            });
            Button.fontColor({
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_text_primary_activated_transparent'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            });
            Button.constraintSize({
                minHeight: BUTTON_SIZE
            });
            Button.backgroundColor({
                'id': -1,
                'type': 10001,
                params: ['sys.color.ohos_id_color_button_normal'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            });
            Button.labelStyle({
                maxLines: TEXT_MAX_LINE
            });
            Button.onFocus(() => {
                this.parentCanFocus = false;
            });
            Button.onHover((isHover) => {
                this.parentCanHover = false;
                if (isHover && this.parentFrontColor === this.hoveringColor && IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.parentFrontColor = this.parentIsActive ? this.activedColor : Color.Transparent.toString();
                }
                if (!isHover) {
                    this.parentCanHover = true;
                    if (this.parentIsHover) {
                        this.parentFrontColor = this.parentIsHover ? this.hoveringColor :
                            (this.parentIsActive ? this.activedColor : Color.Transparent.toString());
                    }
                }
            });
            Button.accessibilityLevel(this.button?.accessibilityLevel ?? ACCESSIBILITY_LEVEL_AUTO);
            Button.accessibilityText(getAccessibilityText(this.button?.accessibilityText ?? ''));
            Button.accessibilityDescription(getAccessibilityText(this.button?.accessibilityDescription ?? ''));
        }, Button);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.padding({
                left: TEXT_SAFE_MARGIN,
                right: TEXT_SAFE_MARGIN
            });
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(this.button?.text);
            Text.focusable(true);
        }, Text);
        Text.pop();
        Row.pop();
        Button.pop();
    }

    createIcon(param, parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Button.createWithChild({ type: ButtonType.Normal });
            Button.shadow(CLEAR_SHADOW);
            Button.hitTestBehavior(IS_SUPPORT_SUBCOMPONENT_EVENT ? HitTestMode.Block : HitTestMode.None);
            Button.backgroundColor(Color.Transparent);
            Button.height(OPERATEITEM_ICON_CLICKABLE_SIZE);
            Button.width(OPERATEITEM_ICON_CLICKABLE_SIZE);
            Button.borderRadius({
                'id': -1,
                'type': 10002,
                params: ['sys.float.ohos_id_corner_radius_clicked'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            });
            Button.onFocus(() => {
                this.parentCanFocus = false;
            });
            Button.onHover((isHover) => {
                this.parentCanHover = false;
                if (isHover && this.parentFrontColor === this.hoveringColor && IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.parentFrontColor = this.parentIsActive ? this.activedColor : Color.Transparent.toString();
                }
                if (!isHover) {
                    this.parentCanHover = true;
                    if (this.parentIsHover) {
                        this.parentFrontColor = this.parentIsHover ? this.hoveringColor :
                            (this.parentIsActive ? this.activedColor : Color.Transparent.toString());
                    }
                }
            });
            Button.onClick(param.icon?.action);
            Button.accessibilityLevel(getAccessibilityLevelOnAction(param.icon?.accessibilityLevel,
                param.icon?.action));
            Button.accessibilityText(getAccessibilityText(param.icon?.accessibilityText ?? ''));
            Button.accessibilityDescription(getAccessibilityText(param.icon?.accessibilityDescription ?? ''));
            Button.flexShrink(0);
        }, Button);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (param.icon?.symbolStyle) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        SymbolGlyph.create();
                        SymbolGlyph.fontColor([this.iconColor]);
                        SymbolGlyph.attributeModifier.bind(this)(param.icon?.symbolStyle);
                        SymbolGlyph.fontSize(`${OPERATEITEM_ICONLIKE_SIZE}vp`);
                        SymbolGlyph.effectStrategy(SymbolEffectStrategy.NONE);
                        SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                        SymbolGlyph.focusable(true);
                        SymbolGlyph.draggable(false);
                    }, SymbolGlyph);
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (Util.isSymbolResource(param.icon?.value)) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create(param.icon?.value);
                                    SymbolGlyph.fontSize(`${OPERATEITEM_ICONLIKE_SIZE}vp`);
                                    SymbolGlyph.fontColor([this.iconColor]);
                                    SymbolGlyph.focusable(true);
                                    SymbolGlyph.draggable(false);
                                }, SymbolGlyph);
                            });
                        } else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    Image.create(param.icon?.value);
                                    Image.height(OPERATEITEM_ICONLIKE_SIZE);
                                    Image.width(OPERATEITEM_ICONLIKE_SIZE);
                                    Image.focusable(true);
                                    Image.fillColor(ObservedObject.GetRawObject(this.iconColor));
                                    Image.draggable(false);
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

    createImage(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (Util.isSymbolResource(ObservedObject.GetRawObject(this.image))) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        SymbolGlyph.create(this.image);
                        SymbolGlyph.fontSize(`${OPERATEITEM_IMAGE_SIZE}vp`);
                        SymbolGlyph.draggable(false);
                        SymbolGlyph.margin({ end: LengthMetrics.vp(LISTITEM_PADDING) });
                    }, SymbolGlyph);
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Image.create(this.image);
                        Image.height(OPERATEITEM_IMAGE_SIZE);
                        Image.width(OPERATEITEM_IMAGE_SIZE);
                        Image.draggable(false);
                        Image.margin({ end: LengthMetrics.vp(LISTITEM_PADDING) });
                    }, Image);
                });
            }
        }, If);
        If.pop();
    }

    createSymbol(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            SymbolGlyph.create();
            SymbolGlyph.attributeModifier.bind(this)(ObservedObject.GetRawObject(this.symbolStyle));
            SymbolGlyph.fontSize(`${OPERATEITEM_IMAGE_SIZE}vp`);
            SymbolGlyph.effectStrategy(SymbolEffectStrategy.NONE);
            SymbolGlyph.symbolEffect(new SymbolEffect(), false);
            SymbolGlyph.draggable(false);
            SymbolGlyph.margin({ end: LengthMetrics.vp(LISTITEM_PADDING) });
        }, SymbolGlyph);
    }

    createText(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(this.text);
            Text.margin({ end: LengthMetrics.vp(LISTITEM_PADDING) });
            Text.fontSize(ObservedObject.GetRawObject(this.secondaryTextSize));
            Text.fontColor(ObservedObject.GetRawObject(this.secondaryTextColors));
            Text.textOverflow({
                overflow: IS_MARQUEE_OR_ELLIPSIS === TEXT_SUPPORT_MARQUEE ? TextOverflow.MARQUEE :
                TextOverflow.None
            });
            Text.marqueeOptions({
                start: this.isFocus || this.parentIsHover,
                fadeout: true,
                marqueeStartPolicy: MarqueeStartPolicy.DEFAULT
            });
            Text.maxLines(LengthMetrics.resource({
                'id': -1,
                'type': 10002,
                params: ['sys.float.composeListItem_maxLines_right'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            }).value);
            Text.draggable(false);
            Text.flexShrink(1);
        }, Text);
        Text.pop();
    }

    createArrow(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Button.createWithChild({ type: ButtonType.Normal });
            Button.shadow(CLEAR_SHADOW);
            Button.margin({ end: LengthMetrics.vp(LISTITEM_PADDING) });
            Button.hitTestBehavior(IS_SUPPORT_SUBCOMPONENT_EVENT ?
                (this.arrow?.action !== undefined ? HitTestMode.Block : HitTestMode.Transparent) : HitTestMode.None);
            Button.backgroundColor(Color.Transparent);
            Button.height(OPERATEITEM_ICONLIKE_SIZE);
            Button.width(OPERATEITEM_ARROW_WIDTH);
            Button.onFocus(() => {
                this.parentCanFocus = false;
            });
            Button.stateEffect(this.arrow?.action !== undefined);
            Button.hoverEffect(this.arrow?.action !== undefined ? HoverEffect.Auto : HoverEffect.None);
            Button.onHover((isHover) => {
                if (this.arrow?.action === undefined) {
                    return;
                }
                if (isHover && IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.parentCanHover = false;
                    this.parentFrontColor = this.parentIsActive ? this.activedColor : Color.Transparent.toString();
                } else {
                    this.parentCanHover = true;
                    if (this.parentIsHover) {
                        this.parentFrontColor = this.parentIsHover ? this.hoveringColor :
                            (this.parentIsActive ? this.activedColor : Color.Transparent.toString());
                    }
                }
            });
            Button.onClick(this.arrow?.action);
            Button.accessibilityLevel(getAccessibilityLevelOnAction(this.arrow?.accessibilityLevel,
                this.arrow?.action));
            Button.accessibilityText(getAccessibilityText(this.arrow?.accessibilityText ?? ''));
            Button.accessibilityDescription(getAccessibilityText(this.arrow?.accessibilityDescription ?? ''));
        }, Button);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.arrow?.symbolStyle) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        SymbolGlyph.create();
                        SymbolGlyph.fontColor([IS_SUPPORT_SUBCOMPONENT_EVENT ? {
                            'id': -1,
                            'type': 10001,
                            params: ['sys.color.ohos_id_color_fourth'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__'
                        } : this.iconColor]);
                        SymbolGlyph.attributeModifier.bind(this)(this.arrow?.symbolStyle);
                        SymbolGlyph.fontSize(`${OPERATEITEM_ICONLIKE_SIZE}vp`);
                        SymbolGlyph.effectStrategy(SymbolEffectStrategy.NONE);
                        SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                        SymbolGlyph.focusable(true);
                        SymbolGlyph.draggable(false);
                    }, SymbolGlyph);
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (Util.isSymbolResource(this.arrow?.value)) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create(this.arrow?.value);
                                    SymbolGlyph.fontSize(`${OPERATEITEM_ICONLIKE_SIZE}vp`);
                                    SymbolGlyph.fontColor([IS_SUPPORT_SUBCOMPONENT_EVENT ? {
                                        'id': -1,
                                        'type': 10001,
                                        params: ['sys.color.ohos_id_color_fourth'],
                                        'bundleName': '__harDefaultBundleName__',
                                        'moduleName': '__harDefaultModuleName__'
                                    } : this.iconColor]);
                                    SymbolGlyph.focusable(true);
                                    SymbolGlyph.draggable(false);
                                }, SymbolGlyph);
                            });
                        } else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    Image.create(this.arrow?.value);
                                    Image.height(OPERATEITEM_ICONLIKE_SIZE);
                                    Image.width(OPERATEITEM_ARROW_WIDTH);
                                    Image.focusable(true);
                                    Image.fillColor(IS_SUPPORT_SUBCOMPONENT_EVENT ? {
                                        'id': -1,
                                        'type': 10001,
                                        params: ['sys.color.ohos_id_color_fourth'],
                                        'bundleName': '__harDefaultBundleName__',
                                        'moduleName': '__harDefaultModuleName__'
                                    } : this.iconColor);
                                    Image.draggable(false);
                                    Image.matchTextDirection(true);
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

    createRadio(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Radio.create({ value: '', group: '' });
            Radio.margin({ end: LengthMetrics.vp(LISTITEM_PADDING) });
            Radio.checked(this.radioState);
            Radio.radioStyle({
                uncheckedBorderColor: this.getUnselectedColor()
            });
            Radio.backgroundColor(!IS_SUPPORT_SUBCOMPONENT_EVENT && this.isFocus ? OPERATE_ITEM_BACKGROUND_COLOR :
            Color.Transparent);
            Radio.borderRadius(OPERATE_ITEM_RADIUS);
            Radio.onChange((isCheck) => {
                if (!IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.radioState = isCheck;
                    this.isChecked = isCheck;
                }
                if (this.radio?.onChange) {
                    this.radio?.onChange(isCheck);
                }
            });
            Radio.height(OPERATEITEM_ICONLIKE_SIZE);
            Radio.width(OPERATEITEM_ICONLIKE_SIZE);
            Radio.padding(OPERATEITEM_SELECTIONBOX_PADDING_SIZE);
            Radio.onFocus(() => {
                this.parentCanFocus = false;
            });
            Radio.hitTestBehavior(IS_SUPPORT_SUBCOMPONENT_EVENT ? HitTestMode.Block : HitTestMode.None);
            Radio.flexShrink(0);
            Radio.onHover((isHover) => {
                this.parentCanHover = false;
                if (isHover && this.parentFrontColor === this.hoveringColor && IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.parentFrontColor = this.parentIsActive ? this.activedColor : Color.Transparent.toString();
                }
                if (!isHover) {
                    this.parentCanHover = true;
                    if (this.parentIsHover) {
                        this.parentFrontColor = this.parentIsHover ? this.hoveringColor :
                            (this.parentIsActive ? this.activedColor : Color.Transparent.toString());
                    }
                }
            });
            Radio.accessibilityLevel(getAccessibilityLevelOnChange(this.radio?.accessibilityLevel,
                this.radio?.onChange));
            Radio.accessibilityText(getAccessibilityText(this.radio?.accessibilityText ?? ''));
            Radio.accessibilityDescription(getAccessibilityText(this.radio?.accessibilityDescription ?? ''));
        }, Radio);
    }

    createCheckBox(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Checkbox.create();
            Checkbox.borderRadius(IS_SUPPORT_SUBCOMPONENT_EVENT ? UNUSUAL : OPERATE_ITEM_RADIUS);
            Checkbox.unselectedColor(this.getUnselectedColor());
            Checkbox.backgroundColor(!IS_SUPPORT_SUBCOMPONENT_EVENT && this.isFocus ? OPERATE_ITEM_BACKGROUND_COLOR :
            Color.Transparent);
            Checkbox.margin({ end: LengthMetrics.vp(LISTITEM_PADDING) });
            Checkbox.select(this.checkBoxState);
            Checkbox.onChange((isCheck) => {
                if (!IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.checkBoxState = isCheck;
                    this.isChecked = isCheck;
                }
                if (this.checkBox?.onChange) {
                    this.checkBox?.onChange(isCheck);
                }
            });
            Checkbox.height(OPERATEITEM_ICONLIKE_SIZE);
            Checkbox.width(OPERATEITEM_ICONLIKE_SIZE);
            Checkbox.padding(OPERATEITEM_SELECTIONBOX_PADDING_SIZE);
            Checkbox.onFocus(() => {
                this.parentCanFocus = false;
            });
            Checkbox.hitTestBehavior(IS_SUPPORT_SUBCOMPONENT_EVENT ? HitTestMode.Block : HitTestMode.None);
            Checkbox.flexShrink(0);
            Checkbox.onHover((isHover) => {
                this.parentCanHover = false;
                if (isHover && this.parentFrontColor === this.hoveringColor && IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.parentFrontColor = this.parentIsActive ? this.activedColor : Color.Transparent.toString();
                }
                if (!isHover) {
                    this.parentCanHover = true;
                    if (this.parentIsHover) {
                        this.parentFrontColor = this.parentIsHover ? this.hoveringColor :
                            (this.parentIsActive ? this.activedColor : Color.Transparent.toString());
                    }
                }
            });
            Checkbox.accessibilityLevel(getAccessibilityLevelOnChange(this.checkBox?.accessibilityLevel,
                this.checkBox?.onChange));
            Checkbox.accessibilityText(getAccessibilityText(this.checkBox?.accessibilityText ?? ''));
            Checkbox.accessibilityDescription(getAccessibilityText(this.checkBox?.accessibilityDescription ?? ''));
        }, Checkbox);
        Checkbox.pop();
    }

    createSwitch(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.margin({ end: LengthMetrics.vp(SWITCH_PADDING) });
            Row.height(OPERATEITEM_ICON_CLICKABLE_SIZE);
            Row.width(OPERATEITEM_ICON_CLICKABLE_SIZE);
            Row.justifyContent(FlexAlign.Center);
            Row.onFocus(() => {
                this.parentCanFocus = false;
            });
            Row.onHover((isHover) => {
                this.parentCanHover = false;
                if (isHover && this.parentFrontColor === this.hoveringColor && IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.parentFrontColor = this.parentIsActive ? this.activedColor : Color.Transparent.toString();
                }
                if (!isHover) {
                    this.parentCanHover = true;
                    if (this.parentIsHover) {
                        this.parentFrontColor = this.parentIsHover ? this.hoveringColor :
                            (this.parentIsActive ? this.activedColor : Color.Transparent.toString());
                    }
                }
            });
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Toggle.create({ type: ToggleType.Switch, isOn: this.switchState });
            Toggle.borderRadius(IS_SUPPORT_SUBCOMPONENT_EVENT ? UNUSUAL : OPERATE_ITEM_RADIUS);
            Toggle.backgroundColor(!IS_SUPPORT_SUBCOMPONENT_EVENT && this.isFocus ? OPERATE_ITEM_BACKGROUND_COLOR :
            Color.Transparent);
            Toggle.switchPointColor(!IS_SUPPORT_SUBCOMPONENT_EVENT && this.isFocus && !this.switchState ?
                OPERATE_ITEM_COLOR :
                UNUSUAL);
            Toggle.onChange((isCheck) => {
                this.switchState = isCheck;
                if (!IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.isChecked = isCheck;
                }
                if (this.switch?.onChange) {
                    this.switch?.onChange(isCheck);
                }
            });
            Toggle.onClick(() => {
                this.switchState = !this.switchState;
            });
            Toggle.hitTestBehavior(IS_SUPPORT_SUBCOMPONENT_EVENT ? HitTestMode.Block : HitTestMode.None);
            Toggle.accessibilityLevel(getAccessibilityLevelOnChange(this.switch?.accessibilityLevel,
                this.switch?.onChange));
            Toggle.accessibilityText(getAccessibilityText(this.switch?.accessibilityText ?? ''));
            Toggle.accessibilityDescription(getAccessibilityText(this.switch?.accessibilityDescription ?? ''));
        }, Toggle);
        Toggle.pop();
        Row.pop();
    }

    createTextArrow(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Button.createWithChild({ type: ButtonType.Normal });
            Button.shadow(CLEAR_SHADOW);
            Button.hitTestBehavior(IS_SUPPORT_SUBCOMPONENT_EVENT ?
                (this.arrow?.action !== undefined ? HitTestMode.Block : HitTestMode.Transparent) : HitTestMode.None);
            Button.labelStyle({
                maxLines: TEXT_MAX_LINE
            });
            Button.backgroundColor(Color.Transparent);
            Button.constraintSize({ minHeight: TEXT_ARROW_HEIGHT });
            Button.borderRadius({
                'id': -1,
                'type': 10002,
                params: ['sys.float.ohos_id_corner_radius_clicked'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            });
            Button.onFocus(() => {
                this.parentCanFocus = false;
            });
            Button.padding({
                top: 0,
                bottom: 0,
                left: 0,
                right: 0
            });
            Button.stateEffect(this.arrow?.action !== undefined);
            Button.hoverEffect(this.arrow?.action !== undefined ? HoverEffect.Auto : HoverEffect.None);
            Button.onHover((isHover) => {
                if (this.arrow?.action === undefined) {
                    return;
                }
                if (isHover && IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.parentCanHover = false;
                    this.parentFrontColor = this.parentIsActive ? this.activedColor : Color.Transparent.toString();
                } else {
                    this.parentCanHover = true;
                    if (this.parentIsHover) {
                        this.parentFrontColor = this.parentIsHover ? this.hoveringColor :
                            (this.parentIsActive ? this.activedColor : Color.Transparent.toString());
                    }
                }
            });
            Button.onClick(this.arrow?.action);
            Button.accessibilityLevel(getAccessibilityLevelOnAction(this.arrow?.accessibilityLevel,
                this.arrow?.action));
            Button.accessibilityText(`${this.text} ${getAccessibilityText(this.arrow?.accessibilityText ?? '')}`);
            Button.accessibilityDescription(getAccessibilityText(this.arrow?.accessibilityDescription ?? ''));
        }, Button);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.parentDirection === FlexDirection.Column) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Flex.create({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center });
                        Flex.padding({
                            start: LengthMetrics.vp(TEXT_SAFE_MARGIN),
                            end: LengthMetrics.vp(LISTITEM_PADDING)
                        });
                    }, Flex);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(this.text);
                        Text.fontSize({
                            'id': -1,
                            'type': 10002,
                            params: ['sys.float.ohos_id_text_size_body2'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__'
                        });
                        Text.fontColor(ObservedObject.GetRawObject(this.secondaryTextColor));
                        Text.focusable(true);
                        Text.draggable(false);
                        Text.constraintSize({
                            maxWidth: `calc(100% - ${OPERATEITEM_ARROW_WIDTH}vp)`
                        });
                    }, Text);
                    Text.pop();
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.arrow?.symbolStyle) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create();
                                    SymbolGlyph.fontColor([{
                                        'id': -1,
                                        'type': 10001,
                                        params: ['sys.color.ohos_id_color_fourth'],
                                        'bundleName': '__harDefaultBundleName__',
                                        'moduleName': '__harDefaultModuleName__'
                                    }]);
                                    SymbolGlyph.attributeModifier.bind(this)(this.arrow?.symbolStyle);
                                    SymbolGlyph.fontSize(`${OPERATEITEM_ICONLIKE_SIZE}vp`);
                                    SymbolGlyph.effectStrategy(SymbolEffectStrategy.NONE);
                                    SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                                    SymbolGlyph.focusable(false);
                                    SymbolGlyph.draggable(false);
                                }, SymbolGlyph);
                            });
                        } else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    If.create();
                                    if (Util.isSymbolResource(this.arrow?.value)) {
                                        this.ifElseBranchUpdateFunction(0, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                SymbolGlyph.create(this.arrow?.value);
                                                SymbolGlyph.fontSize(`${OPERATEITEM_ICONLIKE_SIZE}vp`);
                                                SymbolGlyph.fontColor([{
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.ohos_id_color_fourth'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__'
                                                }]);
                                                SymbolGlyph.focusable(false);
                                                SymbolGlyph.draggable(false);
                                            }, SymbolGlyph);
                                        });
                                    } else {
                                        this.ifElseBranchUpdateFunction(1, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                Image.create(this.arrow?.value);
                                                Image.height(OPERATEITEM_ICONLIKE_SIZE);
                                                Image.width(OPERATEITEM_ARROW_WIDTH);
                                                Image.fillColor({
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.ohos_id_color_fourth'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__'
                                                });
                                                Image.focusable(false);
                                                Image.draggable(false);
                                                Image.matchTextDirection(true);
                                            }, Image);
                                        });
                                    }
                                }, If);
                                If.pop();
                            });
                        }
                    }, If);
                    If.pop();
                    Flex.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Row.create({ space: SPECICAL_ROW_SPACE });
                        Row.padding({
                            start: LengthMetrics.vp(TEXT_SAFE_MARGIN),
                            end: LengthMetrics.vp(LISTITEM_PADDING)
                        });
                    }, Row);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(this.text);
                        Text.fontSize(ObservedObject.GetRawObject(this.secondaryTextSize));
                        Text.fontColor(ObservedObject.GetRawObject(this.secondaryTextColors));
                        Text.textOverflow({
                            overflow: IS_MARQUEE_OR_ELLIPSIS === TEXT_SUPPORT_MARQUEE ? TextOverflow.MARQUEE :
                            TextOverflow.None
                        });
                        Text.marqueeOptions({
                            start: this.isFocus || this.parentIsHover,
                            fadeout: true,
                            marqueeStartPolicy: MarqueeStartPolicy.DEFAULT
                        });
                        Text.maxLines(LengthMetrics.resource({
                            'id': -1,
                            'type': 10002,
                            params: ['sys.float.composeListItem_maxLines_right'],
                            'bundleName': '__harDefaultBundleName__',
                            'moduleName': '__harDefaultModuleName__'
                        }).value);
                        Text.focusable(true);
                        Text.draggable(false);
                        Text.constraintSize({
                            maxWidth: `calc(100% - ${OPERATEITEM_ARROW_WIDTH + OPERATEITEM_ARROW_MARGIN_WIDTH}vp)`
                        });
                        Text.margin({ right: OPERATEITEM_ARROW_MARGIN_WIDTH });
                    }, Text);
                    Text.pop();
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.arrow?.symbolStyle) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    SymbolGlyph.create();
                                    SymbolGlyph.fontColor([IS_SUPPORT_SUBCOMPONENT_EVENT ? {
                                        'id': -1,
                                        'type': 10001,
                                        params: ['sys.color.icon_fourth'],
                                        'bundleName': '__harDefaultBundleName__',
                                        'moduleName': '__harDefaultModuleName__'
                                    } : this.iconColor]);
                                    SymbolGlyph.attributeModifier.bind(this)(this.arrow?.symbolStyle);
                                    SymbolGlyph.fontSize(`${OPERATEITEM_ICONLIKE_SIZE}vp`);
                                    SymbolGlyph.effectStrategy(SymbolEffectStrategy.NONE);
                                    SymbolGlyph.symbolEffect(new SymbolEffect(), false);
                                    SymbolGlyph.focusable(false);
                                    SymbolGlyph.draggable(false);
                                }, SymbolGlyph);
                            });
                        } else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    If.create();
                                    if (Util.isSymbolResource(this.arrow?.value)) {
                                        this.ifElseBranchUpdateFunction(0, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                SymbolGlyph.create(this.arrow?.value);
                                                SymbolGlyph.fontSize(`${OPERATEITEM_ICONLIKE_SIZE}vp`);
                                                SymbolGlyph.fontColor([IS_SUPPORT_SUBCOMPONENT_EVENT ? {
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.icon_fourth'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__'
                                                } : this.iconColor]);
                                                SymbolGlyph.focusable(false);
                                                SymbolGlyph.draggable(false);
                                            }, SymbolGlyph);
                                        });
                                    } else {
                                        this.ifElseBranchUpdateFunction(1, () => {
                                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                                Image.create(this.arrow?.value);
                                                Image.height(OPERATEITEM_ICONLIKE_SIZE);
                                                Image.width(OPERATEITEM_ARROW_WIDTH);
                                                Image.fillColor(IS_SUPPORT_SUBCOMPONENT_EVENT ? {
                                                    'id': -1,
                                                    'type': 10001,
                                                    params: ['sys.color.icon_fourth'],
                                                    'bundleName': '__harDefaultBundleName__',
                                                    'moduleName': '__harDefaultModuleName__'
                                                } : this.iconColor);
                                                Image.focusable(false);
                                                Image.draggable(false);
                                                Image.matchTextDirection(true);
                                            }, Image);
                                        });
                                    }
                                }, If);
                                If.pop();
                            });
                        }
                    }, If);
                    If.pop();
                    Row.pop();
                });
            }
        }, If);
        If.pop();
        Button.pop();
    }

    getFlexOptions() {
        let flexOptions = { alignItems: ItemAlign.Center };
        if (this.parentDirection === FlexDirection.Column) {
            flexOptions.justifyContent = FlexAlign.SpaceBetween;
        } else {
            flexOptions.space = { main: LengthMetrics.vp(this.rowSpace) };
            flexOptions.justifyContent = FlexAlign.End;
        }
        return flexOptions;
    }

    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Flex.create(this.getFlexOptions());
            Flex.width(this.parentDirection === FlexDirection.Column ? undefined : this.rightWidth);
        }, Flex);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.button != null) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.createButton.bind(this)();
                });
            } else if (this.symbolStyle != null) {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.createSymbol.bind(this)();
                });
            } else if (this.image != null) {
                this.ifElseBranchUpdateFunction(2, () => {
                    this.createImage.bind(this)();
                });
            } else if (this.icon != null && this.text != null) {
                this.ifElseBranchUpdateFunction(3, () => {
                    this.createText.bind(this)();
                    this.createIcon.bind(this)(makeBuilderParameterProxy('createIcon',
                        { icon: () => (this['__icon'] ? this['__icon'] : this['icon']) }));
                });
            } else if (this.arrow != null && this.text == null) {
                this.ifElseBranchUpdateFunction(4, () => {
                    this.createArrow.bind(this)();
                });
            } else if (this.arrow != null && this.text != null) {
                this.ifElseBranchUpdateFunction(5, () => {
                    this.createTextArrow.bind(this)();
                });
            } else if (this.text != null) {
                this.ifElseBranchUpdateFunction(6, () => {
                    this.createText.bind(this)();
                });
            } else if (this.radio != null) {
                this.ifElseBranchUpdateFunction(7, () => {
                    this.createRadio.bind(this)();
                });
            } else if (this.checkBox != null) {
                this.ifElseBranchUpdateFunction(8, () => {
                    this.createCheckBox.bind(this)();
                });
            } else if (this.switch != null) {
                this.ifElseBranchUpdateFunction(9, () => {
                    this.createSwitch.bind(this)();
                });
            } else if (this.icon != null) {
                this.ifElseBranchUpdateFunction(10, () => {
                    this.createIcon.bind(this)(makeBuilderParameterProxy('createIcon',
                        { icon: () => (this['__icon'] ? this['__icon'] : this['icon']) }));
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        if (this.subIcon != null) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.createIcon.bind(this)(makeBuilderParameterProxy('createIcon',
                                    { icon: () => (this['__subIcon'] ? this['__subIcon'] : this['subIcon']) }));
                            });
                        } else {
                            this.ifElseBranchUpdateFunction(1, () => {
                            });
                        }
                    }, If);
                    If.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(11, () => {
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

/**
 * Obtain accessible text
 *
 * @param resource initial resource
 * @param selected select state
 * @returns string
 */
function getAccessibilityText(resource) {
    try {
        let resourceString = '';
        if (typeof resource === 'string') {
            resourceString = resource;
        } else {
            resourceString = getContext().resourceManager.getStringSync(resource);
        }
        return resourceString;
    } catch (error) {
        let code = error.code;
        let message = error.message;
        hilog.error(0x3900, 'Ace', `getAccessibilityText error, code: ${code}, message: ${message}`);
        return '';
    }
}

/**
 * Obtain accessible level
 *
 * @param resource
 * @param selected select state
 * @returns string
 */
function getAccessibilityLevelOnChange(accessibilityLevel, onChange) {
    if (accessibilityLevel) {
        return accessibilityLevel;
    }
    if (onChange) {
        return ACCESSIBILITY_LEVEL_YES;
    }
    return ACCESSIBILITY_LEVEL_NO;
}

/**
 * Obtain accessible level
 *
 * @param resource
 * @param selected select state
 * @returns string
 */
function getAccessibilityLevelOnAction(accessibilityLevel, onAction) {
    if (accessibilityLevel) {
        return accessibilityLevel;
    }
    if (onAction) {
        return ACCESSIBILITY_LEVEL_YES;
    }
    return ACCESSIBILITY_LEVEL_NO;
}

export class ComposeListItem extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === 'function') {
            this.paramsGenerator_ = paramsLambda;
        }
        this.__contentItem = new SynchedPropertyObjectOneWayPU(params.contentItem, this, 'contentItem');
        this.__operateItem = new SynchedPropertyObjectOneWayPU(params.operateItem, this, 'operateItem');
        this.__frontColor = new ObservedPropertyObjectPU(NORMAL_BG_COLOR, this, 'frontColor');
        this.__borderSize = new ObservedPropertySimplePU(0, this, 'borderSize');
        this.__canFocus = new ObservedPropertySimplePU(false, this, 'canFocus');
        this.__canTouch = new ObservedPropertySimplePU(true, this, 'canTouch');
        this.__canHover = new ObservedPropertySimplePU(true, this, 'canHover');
        this.__isHover = new ObservedPropertySimplePU(false, this, 'isHover');
        this.__itemHeight = new ObservedPropertySimplePU(ItemHeight.FIRST_HEIGHT, this, 'itemHeight');
        this.__isActive = new ObservedPropertySimplePU(false, this, 'isActive');
        this.__hoveringColor = new ObservedPropertyObjectPU('#0d000000', this, 'hoveringColor');
        this.__touchDownColor = new ObservedPropertyObjectPU('#1a000000', this, 'touchDownColor');
        this.__activedColor = new ObservedPropertyObjectPU('#1a0a59f7', this, 'activedColor');
        this.__focusOutlineColor = new ObservedPropertyObjectPU({
            'id': -1,
            'type': 10001,
            params: ['sys.color.ohos_id_color_focused_outline'],
            'bundleName': '__harDefaultBundleName__',
            'moduleName': '__harDefaultModuleName__'
        }, this, 'focusOutlineColor');
        this.__fontSizeScale = new ObservedPropertySimplePU(1, this, 'fontSizeScale');
        this.__containerDirection = new ObservedPropertySimplePU(FlexDirection.Row, this, 'containerDirection');
        this.__contentItemDirection = new ObservedPropertySimplePU(FlexDirection.Row, this, 'contentItemDirection');
        this.__containerPadding = new ObservedPropertyObjectPU(undefined, this, 'containerPadding');
        this.__textArrowLeftSafeOffset = new ObservedPropertySimplePU(0, this, 'textArrowLeftSafeOffset');
        this.isFollowingSystemFontScale = this.getUIContext().isFollowingSystemFontScale();
        this.maxFontScale = this.getUIContext().getMaxFontScale();
        this.callbackId = undefined;
        this.__accessibilityTextBuilder = new ObservedPropertySimplePU('', this, 'accessibilityTextBuilder');
        this.__isFocus = new ObservedPropertySimplePU(false, this, 'isFocus');
        this.__isChecked = new ObservedPropertySimplePU(false, this, 'isChecked');
        this.__isWrapText = new ObservedPropertySimplePU(false, this, 'isWrapText');
        this.__listScale = new ObservedPropertyObjectPU({ x: 1, y: 1 }, this, 'listScale');
        this.envCallback = {
            onConfigurationUpdated: (config) => {
                if (config === undefined || !this.isFollowingSystemFontScale) {
                    this.fontSizeScale = 1;
                    return;
                }
                try {
                    this.fontSizeScale = Math.min(this.maxFontScale, config.fontSizeScale ?? 1);
                } catch (paramError) {
                    let code = paramError.code;
                    let message = paramError.message;
                    hilog.error(0x3900, 'Ace', `ComposeListItem environmentCallback error: ${code}, ${message}`);
                }
            },
            onMemoryLevel: (level) => {
            }
        };
        this.setInitiallyProvidedValue(params);
        this.declareWatch('contentItem', this.onPropChange);
        this.declareWatch('operateItem', this.onPropChange);
        this.declareWatch('fontSizeScale', this.onFontSizeScaleChange);
        this.declareWatch('isWrapText', this.onWrapChange);
        this.finalizeConstruction();
    }

    setInitiallyProvidedValue(params) {
        if (params.contentItem === undefined) {
            this.__contentItem.set(null);
        }
        if (params.operateItem === undefined) {
            this.__operateItem.set(null);
        }
        if (params.frontColor !== undefined) {
            this.frontColor = params.frontColor;
        }
        if (params.borderSize !== undefined) {
            this.borderSize = params.borderSize;
        }
        if (params.canFocus !== undefined) {
            this.canFocus = params.canFocus;
        }
        if (params.canTouch !== undefined) {
            this.canTouch = params.canTouch;
        }
        if (params.canHover !== undefined) {
            this.canHover = params.canHover;
        }
        if (params.isHover !== undefined) {
            this.isHover = params.isHover;
        }
        if (params.itemHeight !== undefined) {
            this.itemHeight = params.itemHeight;
        }
        if (params.isActive !== undefined) {
            this.isActive = params.isActive;
        }
        if (params.hoveringColor !== undefined) {
            this.hoveringColor = params.hoveringColor;
        }
        if (params.touchDownColor !== undefined) {
            this.touchDownColor = params.touchDownColor;
        }
        if (params.activedColor !== undefined) {
            this.activedColor = params.activedColor;
        }
        if (params.focusOutlineColor !== undefined) {
            this.focusOutlineColor = params.focusOutlineColor;
        }
        if (params.fontSizeScale !== undefined) {
            this.fontSizeScale = params.fontSizeScale;
        }
        if (params.containerDirection !== undefined) {
            this.containerDirection = params.containerDirection;
        }
        if (params.contentItemDirection !== undefined) {
            this.contentItemDirection = params.contentItemDirection;
        }
        if (params.containerPadding !== undefined) {
            this.containerPadding = params.containerPadding;
        }
        if (params.textArrowLeftSafeOffset !== undefined) {
            this.textArrowLeftSafeOffset = params.textArrowLeftSafeOffset;
        }
        if (params.isFollowingSystemFontScale !== undefined) {
            this.isFollowingSystemFontScale = params.isFollowingSystemFontScale;
        }
        if (params.maxFontScale !== undefined) {
            this.maxFontScale = params.maxFontScale;
        }
        if (params.callbackId !== undefined) {
            this.callbackId = params.callbackId;
        }
        if (params.accessibilityTextBuilder !== undefined) {
            this.accessibilityTextBuilder = params.accessibilityTextBuilder;
        }
        if (params.isFocus !== undefined) {
            this.isFocus = params.isFocus;
        }
        if (params.isChecked !== undefined) {
            this.isChecked = params.isChecked;
        }
        if (params.isWrapText !== undefined) {
            this.isWrapText = params.isWrapText;
        }
        if (params.listScale !== undefined) {
            this.listScale = params.listScale;
        }
        if (params.envCallback !== undefined) {
            this.envCallback = params.envCallback;
        }
    }

    updateStateVars(params) {
        this.__contentItem.reset(params.contentItem);
        this.__operateItem.reset(params.operateItem);
    }

    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__contentItem.purgeDependencyOnElmtId(rmElmtId);
        this.__operateItem.purgeDependencyOnElmtId(rmElmtId);
        this.__frontColor.purgeDependencyOnElmtId(rmElmtId);
        this.__borderSize.purgeDependencyOnElmtId(rmElmtId);
        this.__canFocus.purgeDependencyOnElmtId(rmElmtId);
        this.__canTouch.purgeDependencyOnElmtId(rmElmtId);
        this.__canHover.purgeDependencyOnElmtId(rmElmtId);
        this.__isHover.purgeDependencyOnElmtId(rmElmtId);
        this.__itemHeight.purgeDependencyOnElmtId(rmElmtId);
        this.__isActive.purgeDependencyOnElmtId(rmElmtId);
        this.__hoveringColor.purgeDependencyOnElmtId(rmElmtId);
        this.__touchDownColor.purgeDependencyOnElmtId(rmElmtId);
        this.__activedColor.purgeDependencyOnElmtId(rmElmtId);
        this.__focusOutlineColor.purgeDependencyOnElmtId(rmElmtId);
        this.__fontSizeScale.purgeDependencyOnElmtId(rmElmtId);
        this.__containerDirection.purgeDependencyOnElmtId(rmElmtId);
        this.__contentItemDirection.purgeDependencyOnElmtId(rmElmtId);
        this.__containerPadding.purgeDependencyOnElmtId(rmElmtId);
        this.__textArrowLeftSafeOffset.purgeDependencyOnElmtId(rmElmtId);
        this.__accessibilityTextBuilder.purgeDependencyOnElmtId(rmElmtId);
        this.__isFocus.purgeDependencyOnElmtId(rmElmtId);
        this.__isChecked.purgeDependencyOnElmtId(rmElmtId);
        this.__isWrapText.purgeDependencyOnElmtId(rmElmtId);
        this.__listScale.purgeDependencyOnElmtId(rmElmtId);
    }

    aboutToBeDeleted() {
        this.__contentItem.aboutToBeDeleted();
        this.__operateItem.aboutToBeDeleted();
        this.__frontColor.aboutToBeDeleted();
        this.__borderSize.aboutToBeDeleted();
        this.__canFocus.aboutToBeDeleted();
        this.__canTouch.aboutToBeDeleted();
        this.__canHover.aboutToBeDeleted();
        this.__isHover.aboutToBeDeleted();
        this.__itemHeight.aboutToBeDeleted();
        this.__isActive.aboutToBeDeleted();
        this.__hoveringColor.aboutToBeDeleted();
        this.__touchDownColor.aboutToBeDeleted();
        this.__activedColor.aboutToBeDeleted();
        this.__focusOutlineColor.aboutToBeDeleted();
        this.__fontSizeScale.aboutToBeDeleted();
        this.__containerDirection.aboutToBeDeleted();
        this.__contentItemDirection.aboutToBeDeleted();
        this.__containerPadding.aboutToBeDeleted();
        this.__textArrowLeftSafeOffset.aboutToBeDeleted();
        this.__accessibilityTextBuilder.aboutToBeDeleted();
        this.__isFocus.aboutToBeDeleted();
        this.__isChecked.aboutToBeDeleted();
        this.__isWrapText.aboutToBeDeleted();
        this.__listScale.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }

    get contentItem() {
        return this.__contentItem.get();
    }

    set contentItem(newValue) {
        this.__contentItem.set(newValue);
    }

    get operateItem() {
        return this.__operateItem.get();
    }

    set operateItem(newValue) {
        this.__operateItem.set(newValue);
    }

    get frontColor() {
        return this.__frontColor.get();
    }

    set frontColor(newValue) {
        this.__frontColor.set(newValue);
    }

    get borderSize() {
        return this.__borderSize.get();
    }

    set borderSize(newValue) {
        this.__borderSize.set(newValue);
    }

    get canFocus() {
        return this.__canFocus.get();
    }

    set canFocus(newValue) {
        this.__canFocus.set(newValue);
    }

    get canTouch() {
        return this.__canTouch.get();
    }

    set canTouch(newValue) {
        this.__canTouch.set(newValue);
    }

    get canHover() {
        return this.__canHover.get();
    }

    set canHover(newValue) {
        this.__canHover.set(newValue);
    }

    get isHover() {
        return this.__isHover.get();
    }

    set isHover(newValue) {
        this.__isHover.set(newValue);
    }

    get itemHeight() {
        return this.__itemHeight.get();
    }

    set itemHeight(newValue) {
        this.__itemHeight.set(newValue);
    }

    get isActive() {
        return this.__isActive.get();
    }

    set isActive(newValue) {
        this.__isActive.set(newValue);
    }

    get hoveringColor() {
        return this.__hoveringColor.get();
    }

    set hoveringColor(newValue) {
        this.__hoveringColor.set(newValue);
    }

    get touchDownColor() {
        return this.__touchDownColor.get();
    }

    set touchDownColor(newValue) {
        this.__touchDownColor.set(newValue);
    }

    get activedColor() {
        return this.__activedColor.get();
    }

    set activedColor(newValue) {
        this.__activedColor.set(newValue);
    }

    get focusOutlineColor() {
        return this.__focusOutlineColor.get();
    }

    set focusOutlineColor(newValue) {
        this.__focusOutlineColor.set(newValue);
    }

    get fontSizeScale() {
        return this.__fontSizeScale.get();
    }

    set fontSizeScale(newValue) {
        this.__fontSizeScale.set(newValue);
    }

    get containerDirection() {
        return this.__containerDirection.get();
    }

    set containerDirection(newValue) {
        this.__containerDirection.set(newValue);
    }

    get contentItemDirection() {
        return this.__contentItemDirection.get();
    }

    set contentItemDirection(newValue) {
        this.__contentItemDirection.set(newValue);
    }

    get containerPadding() {
        return this.__containerPadding.get();
    }

    set containerPadding(newValue) {
        this.__containerPadding.set(newValue);
    }

    get textArrowLeftSafeOffset() {
        return this.__textArrowLeftSafeOffset.get();
    }

    set textArrowLeftSafeOffset(newValue) {
        this.__textArrowLeftSafeOffset.set(newValue);
    }

    get accessibilityTextBuilder() {
        return this.__accessibilityTextBuilder.get();
    }

    set accessibilityTextBuilder(newValue) {
        this.__accessibilityTextBuilder.set(newValue);
    }

    get isFocus() {
        return this.__isFocus.get();
    }

    set isFocus(newValue) {
        this.__isFocus.set(newValue);
    }

    get isChecked() {
        return this.__isChecked.get();
    }

    set isChecked(newValue) {
        this.__isChecked.set(newValue);
    }

    get isWrapText() {
        return this.__isWrapText.get();
    }

    set isWrapText(newValue) {
        this.__isWrapText.set(newValue);
    }

    get listScale() {
        return this.__listScale.get();
    }

    set listScale(newValue) {
        this.__listScale.set(newValue);
    }

    onWillApplyTheme(theme) {
        this.hoveringColor = theme.colors.interactiveHover;
        this.touchDownColor = theme.colors.interactivePressed;
        this.activedColor = theme.colors.interactiveActive;
        this.focusOutlineColor = theme.colors.interactiveFocus;
    }

    onWrapChange() {
        this.containerPadding = this.getPadding();
    }

    onPropChange() {
        this.containerDirection = this.decideContainerDirection();
        this.contentItemDirection = this.decideContentItemDirection();
        if (this.contentItem === undefined) {
            if (this.operateItem?.image !== undefined ||
                this.operateItem?.symbolStyle !== undefined ||
                this.operateItem?.icon !== undefined ||
                this.operateItem?.subIcon !== undefined) {
                this.itemHeight = OPERATEITEM_IMAGE_SIZE + SAFE_LIST_PADDING;
            }
            return;
        }
        if (this.contentItem?.secondaryText === undefined && this.contentItem?.description === undefined) {
            if (this.contentItem?.icon === undefined) {
                this.itemHeight = ItemHeight.FIRST_HEIGHT;
            } else {
                this.itemHeight = this.contentItem.iconStyle <= IconType.HEAD_SCULPTURE ?
                ItemHeight.SECOND_HEIGHT :
                    (LengthMetrics.resource({
                        'id': -1,
                        'type': 10002,
                        params: ['sys.float.composeListItem_system_icon_line_height'],
                        'bundleName': '__harDefaultBundleName__',
                        'moduleName': '__harDefaultModuleName__'
                    }).value);
            }
        } else if (this.contentItem.description === undefined) {
            let iconStyle = this.contentItem.iconStyle;
            if (this.contentItem.icon === undefined ||
                (this.contentItem.icon !== undefined && iconStyle <= IconType.SYSTEM_ICON)) {
                this.itemHeight = ItemHeight.THIRD_HEIGHT;
            } else {
                this.itemHeight = iconStyle === IconType.HEAD_SCULPTURE ? ItemHeight.FOURTH_HEIGHT : APPICON_ITEMLENGTH;
            }
        } else {
            this.itemHeight = ItemHeight.FIFTH_HEIGHT;
        }
        if (ICON_SIZE_MAP.get(this.contentItem?.iconStyle) >= this.itemHeight) {
            this.itemHeight = ICON_SIZE_MAP.get(this.contentItem?.iconStyle) + SAFE_LIST_PADDING;
        }
        if (this.operateItem?.arrow && this.operateItem?.text && this.operateItem?.arrow?.action) {
            this.accessibilityTextBuilder = `
        ${getAccessibilityText(this.contentItem?.primaryText ?? '')}
        ${getAccessibilityText(this.contentItem?.secondaryText ?? '')}
        ${getAccessibilityText(this.contentItem?.description ?? '')}
      `;
        } else {
            this.accessibilityTextBuilder = `
        ${getAccessibilityText(this.contentItem?.primaryText ?? '')}
        ${getAccessibilityText(this.contentItem?.secondaryText ?? '')}
        ${getAccessibilityText(this.contentItem?.description ?? '')}
        ${getAccessibilityText(this.operateItem?.text ?? '')}
      `;
        }
    }

    aboutToAppear() {
        this.fontSizeScale = this.decideFontSizeScale();
        this.onPropChange();
        try {
            this.callbackId = getContext()?.getApplicationContext()?.on('environment', this.envCallback);
        } catch (paramError) {
            let code = paramError.code;
            let message = paramError.message;
            hilog.error(0x3900, 'Ace', `ComposeListItem Faild to get environment param error: ${code}, ${message}`);
        }
        if (!IS_SUPPORT_SUBCOMPONENT_EVENT) {
            this.onFontSizeScaleChange();
        }
    }

    aboutToDisappear() {
        if (this.callbackId) {
            this.getUIContext()
            ?.getHostContext()
            ?.getApplicationContext()
            ?.off('environment', this.callbackId);
            this.callbackId = void (0);
        }
    }

    calculatedRightWidth() {
        if (this.operateItem?.text || this.operateItem?.button) {
            return RIGHT_PART_WIDTH;
        }
        if (this.operateItem?.switch) {
            return RIGHT_ONLY_SWITCH_WIDTH;
        } else if (this.operateItem?.checkbox) {
            return RIGHT_ONLY_CHECKBOX_WIDTH;
        } else if (this.operateItem?.radio) {
            return RIGHT_ONLY_RADIO_WIDTH;
        } else if (this.operateItem?.icon) {
            if (this.operateItem?.subIcon) {
                return RIGHT_ICON_SUB_ICON_WIDTH;
            }
            return RIGHT_ONLY_ICON_WIDTH;
        } else if (this.operateItem?.symbolStyle) {
            return RIGHT_ONLY_IMAGE_WIDTH;
        } else if (this.operateItem?.image) {
            return RIGHT_ONLY_IMAGE_WIDTH;
        } else if (this.operateItem?.arrow) {
            return RIGHT_ONLY_ARROW_WIDTH;
        }
        return RIGHT_CONTENT_NULL_RIGHTWIDTH;
    }

    decideContentItemDirection() {
        if (this.fontSizeScale >= FontSizeScaleLevel.LEVEL1 &&
            this.contentItem?.iconStyle && this.contentItem?.iconStyle > IconType.HEAD_SCULPTURE) {
            return FlexDirection.Column;
        }
        return FlexDirection.Row;
    }

    decideContainerDirection() {
        if (this.fontSizeScale < FontSizeScaleLevel.LEVEL1 || !this.contentItem) {
            return FlexDirection.Row;
        }
        if (this.operateItem?.button) {
            return FlexDirection.Column;
        } else if (this.operateItem?.symbolStyle) {
            return FlexDirection.Row;
        } else if (this.operateItem?.image) {
            return FlexDirection.Row;
        } else if (this.operateItem?.icon && this.operateItem?.text) {
            return FlexDirection.Column;
        } else if (this.operateItem?.arrow) {
            if (!this.operateItem?.text) {
                return FlexDirection.Row;
            }
            this.textArrowLeftSafeOffset = TEXT_SAFE_MARGIN;
            return FlexDirection.Column;
        } else if (this.operateItem?.text) {
            return FlexDirection.Column;
        } else {
            return FlexDirection.Row;
        }
    }

    onFontSizeScaleChange() {
        this.containerDirection = this.decideContainerDirection();
        this.contentItemDirection = this.decideContentItemDirection();
        if (this.fontSizeScale >= FontSizeScaleLevel.LEVEL3) {
            this.containerPadding = {
                top: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.padding_level12'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
                bottom: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.padding_level12'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
            };
        } else if (this.fontSizeScale >= FontSizeScaleLevel.LEVEL2) {
            this.containerPadding = {
                top: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.padding_level10'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
                bottom: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.padding_level10'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
            };
        } else if (this.fontSizeScale >= FontSizeScaleLevel.LEVEL1) {
            this.containerPadding = {
                top: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.padding_level8'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
                bottom: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.padding_level8'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
            };
        } else {
            this.containerPadding = this.getPadding();
        }
    }

    isSingleLine() {
        return !this.contentItem?.secondaryText && !this.contentItem?.description;
    }

    getOperateOffset() {
        if (this.containerDirection === FlexDirection.Row) {
            return LengthMetrics.vp(0);
        }
        let iconSize = ICON_SIZE_MAP.get(this.contentItem?.iconStyle);
        if (this.contentItem?.icon && iconSize && iconSize <= HEADSCULPTURE_SIZE) {
            return LengthMetrics.vp(iconSize + NORMAL_ITEM_ROW_SPACE + LISTITEM_PADDING - this.textArrowLeftSafeOffset);
        }
        return LengthMetrics.vp(LISTITEM_PADDING - this.textArrowLeftSafeOffset);
    }

    getMainSpace() {
        if (this.containerDirection === FlexDirection.Column) {
            return LengthMetrics.resource(this.isSingleLine() ? {
                'id': -1,
                'type': 10002,
                params: ['sys.float.padding_level1'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            } : {
                'id': -1,
                'type': 10002,
                params: ['sys.float.padding_level8'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            });
        }
        return LengthMetrics.vp(0);
    }

    getFlexOptions() {
        if (this.containerDirection === FlexDirection.Column) {
            return {
                space: { main: this.getMainSpace() },
                justifyContent: FlexAlign.Center,
                alignItems: ItemAlign.Start,
                direction: this.containerDirection,
            };
        }
        return {
            justifyContent: FlexAlign.SpaceBetween,
            alignItems: ItemAlign.Center,
            direction: this.containerDirection,
        };
    }

    decideFontSizeScale() {
        if (!this.isFollowingSystemFontScale) {
            return 1;
        }
        return Math.min(this.maxFontScale, this.getUIContext().getHostContext()?.config.fontSizeScale ?? 1);
    }

    getPadding() {
        if (!IS_SUPPORT_SUBCOMPONENT_EVENT) {
            let paddingNum = LengthMetrics.resource(ITEM_PADDING).value;
            let compareSize = paddingNum > LISTITEM_PADDING;
            let horizontalPadding = compareSize ? paddingNum - LISTITEM_PADDING : 0;
            return {
                top: this.isWrapText ? paddingNum : 0,
                bottom: this.isWrapText ? paddingNum : 0,
                left: horizontalPadding,
                right: horizontalPadding
            };
        } else {
            return undefined;
        }
    }

    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Stack.create();
            Stack.width('100%');
            Stack.accessibilityGroup(true);
            Stack.accessibilityText(this.accessibilityTextBuilder);
            Stack.onFocus(() => {
                this.isFocus = true;
                this.frontColor = FOCUSED_BG_COLOR;
                this.zoomIn();
            });
            Stack.onBlur(() => {
                this.isFocus = false;
                this.frontColor = NORMAL_BG_COLOR;
                this.zoomOut();
            });
            Stack.borderRadius(IS_SUPPORT_SUBCOMPONENT_EVENT ? undefined : {
                'id': -1,
                'type': 10002,
                params: ['sys.float.composeListItem_radius'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            });
            Stack.onClick(IS_SUPPORT_SUBCOMPONENT_EVENT ? undefined : () => {
                this.isChecked = this.operateItem?.radio ? true : !this.isChecked;
                if (this.operateItem?.icon && this.operateItem.icon?.action) {
                    this.operateItem.icon.action();
                }
                if (this.operateItem?.subIcon && this.operateItem.subIcon?.action) {
                    this.operateItem.subIcon.action();
                }
                if (this.operateItem?.arrow && this.operateItem.arrow?.action) {
                    this.operateItem.arrow.action();
                }
            });
            Stack.scale(ObservedObject.GetRawObject(this.listScale));
            Stack.shadow(IS_SUPPORT_SUBCOMPONENT_EVENT ? undefined : (this.isFocus ? FOCUSED_SHADOW : NORMAL_SHADOW));
            Stack.margin({
                left: !IS_SUPPORT_SUBCOMPONENT_EVENT ? STACK_PADDING : undefined,
                right: !IS_SUPPORT_SUBCOMPONENT_EVENT ? STACK_PADDING : undefined
            });
            Stack.padding({
                left: IS_SUPPORT_SUBCOMPONENT_EVENT ? STACK_PADDING : 0,
                right: IS_SUPPORT_SUBCOMPONENT_EVENT ? STACK_PADDING : 0
            });
        }, Stack);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Flex.create(this.getFlexOptions());
            Flex.height(this.containerDirection === FlexDirection.Column ? 'auto' : undefined);
            Flex.constraintSize({
                minHeight: this.itemHeight
            });
            Flex.focusable(IS_SUPPORT_SUBCOMPONENT_EVENT);
            Flex.borderRadius({
                'id': -1,
                'type': 10002,
                params: ['sys.float.composeListItem_radius'],
                'bundleName': '__harDefaultBundleName__',
                'moduleName': '__harDefaultModuleName__'
            });
            Flex.backgroundColor(ObservedObject.GetRawObject(this.frontColor));
            Flex.onFocus(() => {
                this.canFocus = true;
            });
            Flex.onBlur(() => {
                this.canFocus = false;
            });
            Flex.onHover((isHover) => {
                if (this.isFocus && !IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.isHover = false;
                    return;
                }
                this.isHover = isHover;
                if (this.canHover) {
                    this.frontColor = isHover ? this.hoveringColor :
                        (this.isActive ? this.activedColor : Color.Transparent.toString());
                }
                if (!IS_SUPPORT_SUBCOMPONENT_EVENT) {
                    this.frontColor = isHover ? FOCUSED_BG_COLOR : NORMAL_BG_COLOR;
                    isHover ? this.zoomIn() : this.zoomOut();
                }
            });
            ViewStackProcessor.visualState('focused');
            Flex.border({
                radius: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.composeListItem_radius'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
                width: ITEM_BORDER_SHOWN,
                color: this.focusOutlineColor,
                style: BorderStyle.Solid
            });
            ViewStackProcessor.visualState('normal');
            Flex.border({
                radius: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.composeListItem_radius'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
                color: {
                    'id': -1,
                    'type': 10001,
                    params: ['sys.color.composeListItem_stroke_normal_color'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
                width: {
                    'id': -1,
                    'type': 10002,
                    params: ['sys.float.composeListItem_stroke_normal_thickness'],
                    'bundleName': '__harDefaultBundleName__',
                    'moduleName': '__harDefaultModuleName__'
                },
            });
            ViewStackProcessor.visualState('pressed');
            Flex.backgroundColor(ObservedObject.GetRawObject(this.touchDownColor));
            ViewStackProcessor.visualState();
            Flex.padding(ObservedObject.GetRawObject(this.containerPadding));
        }, Flex);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.contentItem === null) {
                this.ifElseBranchUpdateFunction(0, () => {
                    {
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            if (isInitialRender) {
                                let componentCall = new ContentItemStruct(this, {
                                    isWrapText: this.__isWrapText
                                }, undefined, elmtId, () => {
                                }, {
                                    page: 'library/src/main/ets/components/composelistitem.ets',
                                    line: 1405,
                                    col: 11
                                });
                                ViewPU.create(componentCall);
                                let paramsLambda = () => {
                                    return {
                                        isWrapText: this.isWrapText
                                    };
                                };
                                componentCall.paramsGenerator_ = paramsLambda;
                            } else {
                                this.updateStateVarsOfChildByElmtId(elmtId, {});
                            }
                        }, { name: 'ContentItemStruct' });
                    }
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.contentItem !== null) {
                this.ifElseBranchUpdateFunction(0, () => {
                    {
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            if (isInitialRender) {
                                let componentCall = new ContentItemStruct(this, {
                                    icon: this.contentItem?.icon,
                                    symbolStyle: this.contentItem?.symbolStyle,
                                    iconStyle: this.contentItem?.iconStyle,
                                    primaryText: this.contentItem?.primaryText,
                                    secondaryText: this.contentItem?.secondaryText,
                                    description: this.contentItem?.description,
                                    fontSizeScale: this.fontSizeScale,
                                    parentDirection: this.containerDirection,
                                    itemDirection: this.contentItemDirection,
                                    isFocus: this.isFocus,
                                    itemHeight: this.itemHeight,
                                    isWrapText: this.__isWrapText
                                }, undefined, elmtId, () => {
                                }, {
                                    page: 'library/src/main/ets/components/composelistitem.ets',
                                    line: 1410,
                                    col: 11
                                });
                                ViewPU.create(componentCall);
                                let paramsLambda = () => {
                                    return {
                                        icon: this.contentItem?.icon,
                                        symbolStyle: this.contentItem?.symbolStyle,
                                        iconStyle: this.contentItem?.iconStyle,
                                        primaryText: this.contentItem?.primaryText,
                                        secondaryText: this.contentItem?.secondaryText,
                                        description: this.contentItem?.description,
                                        fontSizeScale: this.fontSizeScale,
                                        parentDirection: this.containerDirection,
                                        itemDirection: this.contentItemDirection,
                                        isFocus: this.isFocus,
                                        itemHeight: this.itemHeight,
                                        isWrapText: this.isWrapText
                                    };
                                };
                                componentCall.paramsGenerator_ = paramsLambda;
                            } else {
                                this.updateStateVarsOfChildByElmtId(elmtId, {
                                    icon: this.contentItem?.icon,
                                    symbolStyle: this.contentItem?.symbolStyle,
                                    iconStyle: this.contentItem?.iconStyle,
                                    primaryText: this.contentItem?.primaryText,
                                    secondaryText: this.contentItem?.secondaryText,
                                    description: this.contentItem?.description,
                                    fontSizeScale: this.fontSizeScale,
                                    parentDirection: this.containerDirection,
                                    itemDirection: this.contentItemDirection,
                                    isFocus: this.isFocus,
                                    itemHeight: this.itemHeight
                                });
                            }
                        }, { name: 'ContentItemStruct' });
                    }
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (this.operateItem !== null) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        __Common__.create();
                        __Common__.flexShrink(0);
                        __Common__.onFocus(() => {
                            this.canFocus = false;
                        });
                        __Common__.onBlur(() => {
                            this.canFocus = true;
                        });
                        __Common__.padding({ start: this.getOperateOffset() });
                    }, __Common__);
                    {
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            if (isInitialRender) {
                                let componentCall = new OperateItemStruct(this, {
                                    icon: this.operateItem?.icon,
                                    subIcon: this.operateItem?.subIcon,
                                    button: this.operateItem?.button,
                                    switch: this.operateItem?.switch,
                                    checkBox: this.operateItem?.checkbox,
                                    radio: this.operateItem?.radio,
                                    image: this.operateItem?.image,
                                    symbolStyle: this.operateItem?.symbolStyle,
                                    text: this.operateItem?.text,
                                    arrow: this.operateItem?.arrow,
                                    parentCanFocus: this.__canFocus,
                                    parentCanTouch: this.__canTouch,
                                    parentIsHover: this.__isHover,
                                    parentFrontColor: this.__frontColor,
                                    parentIsActive: this.__isActive,
                                    parentCanHover: this.__canHover,
                                    rightWidth: this.calculatedRightWidth(),
                                    parentDirection: this.__containerDirection,
                                    isFocus: this.__isFocus,
                                    isChecked: this.__isChecked,
                                }, undefined, elmtId, () => {
                                }, {
                                    page: 'library/src/main/ets/components/composelistitem.ets',
                                    line: 1426,
                                    col: 11
                                });
                                ViewPU.create(componentCall);
                                let paramsLambda = () => {
                                    return {
                                        icon: this.operateItem?.icon,
                                        subIcon: this.operateItem?.subIcon,
                                        button: this.operateItem?.button,
                                        switch: this.operateItem?.switch,
                                        checkBox: this.operateItem?.checkbox,
                                        radio: this.operateItem?.radio,
                                        image: this.operateItem?.image,
                                        symbolStyle: this.operateItem?.symbolStyle,
                                        text: this.operateItem?.text,
                                        arrow: this.operateItem?.arrow,
                                        parentCanFocus: this.canFocus,
                                        parentCanTouch: this.canTouch,
                                        parentIsHover: this.isHover,
                                        parentFrontColor: this.frontColor,
                                        parentIsActive: this.isActive,
                                        parentCanHover: this.canHover,
                                        rightWidth: this.calculatedRightWidth(),
                                        parentDirection: this.containerDirection,
                                        isFocus: this.isFocus,
                                        isChecked: this.isChecked
                                    };
                                };
                                componentCall.paramsGenerator_ = paramsLambda;
                            } else {
                                this.updateStateVarsOfChildByElmtId(elmtId, {
                                    icon: this.operateItem?.icon,
                                    subIcon: this.operateItem?.subIcon,
                                    button: this.operateItem?.button,
                                    switch: this.operateItem?.switch,
                                    checkBox: this.operateItem?.checkbox,
                                    radio: this.operateItem?.radio,
                                    image: this.operateItem?.image,
                                    symbolStyle: this.operateItem?.symbolStyle,
                                    text: this.operateItem?.text,
                                    arrow: this.operateItem?.arrow,
                                    rightWidth: this.calculatedRightWidth()
                                });
                            }
                        }, { name: 'OperateItemStruct' });
                    }
                    __Common__.pop();
                });
            } else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Flex.pop();
        Stack.pop();
    }

    zoomIn() {
        this.listScale = {
            x: IS_SUPPORT_SUBCOMPONENT_EVENT ? undefined : FOCUSED_ITEM_SCALE,
            y: IS_SUPPORT_SUBCOMPONENT_EVENT ? undefined : FOCUSED_ITEM_SCALE
        };
    }

    zoomOut() {
        this.listScale = {
            x: IS_SUPPORT_SUBCOMPONENT_EVENT ? undefined : RECOVER_ITEM_SCALE,
            y: IS_SUPPORT_SUBCOMPONENT_EVENT ? undefined : RECOVER_ITEM_SCALE
        };
    }

    rerender() {
        this.updateDirtyElements();
    }
}

export default {
    IconType, ComposeListItem
};