/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/jsview/js_indexer.h"

#include "base/geometry/dimension.h"
#include "base/log/ace_scoring_log.h"
#include "base/utils/utils.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_scroller.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/jsview/models/indexer_model_impl.h"
#include "bridge/declarative_frontend/ark_theme/theme_apply/js_indexer_theme.h"
#include "core/common/resource/resource_object.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/indexer/indexer_theme.h"
#include "core/components_ng/pattern/indexer/indexer_model_ng.h"

namespace OHOS::Ace {
std::unique_ptr<IndexerModel> IndexerModel::instance_ = nullptr;
std::mutex IndexerModel::mutex_;
IndexerModel* IndexerModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::IndexerModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::IndexerModelNG());
            } else {
                instance_.reset(new Framework::IndexerModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}
} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {
namespace {
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const std::vector<V2::AlignStyle> ALIGN_STYLE = { V2::AlignStyle::LEFT, V2::AlignStyle::RIGHT, V2::AlignStyle::START,
    V2::AlignStyle::END };
const std::vector<NG::AlignStyle> NG_ALIGN_STYLE = { NG::AlignStyle::LEFT, NG::AlignStyle::RIGHT, NG::AlignStyle::START,
    NG::AlignStyle::END };
constexpr Dimension DEFAULT_ITEM_SIZE = 16.0_vp;
constexpr double ZERO_RADIUS = 0.0;
constexpr double POPUP_ITEM_DEFAULT_RADIUS = 24.0;
constexpr double ITEM_DEFAULT_RADIUS = 8.0;
constexpr double RADIUS_OFFSET = 4.0;
}; // namespace

void JSIndexer::ParseIndexerSelectedObject(
    const JSCallbackInfo& info, const JSRef<JSVal>& changeEventVal, bool isMethodProp = false)
{
    CHECK_NULL_VOID(changeEventVal->IsFunction());
    auto vm = info.GetVm();
    auto jsFunc = JSRef<JSFunc>::Cast(changeEventVal);
    auto func = jsFunc->GetLocalHandle();
    auto changeEvent = [vm, func = panda::CopyableGlobal(vm, func)](const int32_t selected) {
        panda::LocalScope pandaScope(vm);
        panda::TryCatch trycatch(vm);
        ACE_SCORING_EVENT("Indexer.SelectedChangeEvent");
        panda::Local<panda::JSValueRef> params[1] = { panda::NumberRef::New(vm, selected) };
        func->Call(vm, func.ToLocal(), params, 1);
    };

    if (isMethodProp) {
        IndexerModel::GetInstance()->SetChangeEvent(changeEvent);
    } else {
        IndexerModel::GetInstance()->SetCreatChangeEvent(changeEvent);
    }
}

void JSIndexer::CreateArc(const JSCallbackInfo& args)
{
    Create(args, true);
}

void JSIndexer::Create(const JSCallbackInfo& args)
{
    Create(args, false);
}

void JSIndexer::Create(const JSCallbackInfo& args, bool isArc)
{
    if (args.Length() < 1 || !args[0]->IsObject()) {
        return;
    }
    size_t length = 0;
    int32_t selectedVal = 0;
    std::vector<std::string> indexerArray;
    JSRef<JSObject> paramObj = JSRef<JSObject>::Cast(args[0]);
    JSRef<JSVal> arrayVal = paramObj->GetProperty("arrayValue");
    if (arrayVal->IsArray()) {
        JSRef<JSArray> jsArray = JSRef<JSArray>::Cast(arrayVal);
        length = jsArray->Length();
        for (size_t i = 0; i < length; i++) {
            auto value = jsArray->GetValueAt(i);
            if (value->IsString()) {
                indexerArray.emplace_back(value->ToString());
            }
        }
    }

    JSRef<JSVal> selectedProperty = paramObj->GetProperty("selected");
    if (selectedProperty->IsNumber()) {
        selectedVal = selectedProperty->ToNumber<int32_t>();
        IndexerModel::GetInstance()->Create(indexerArray, selectedVal, isArc);
        JSIndexerTheme::ApplyTheme();
        JSRef<JSVal> changeEventVal = paramObj->GetProperty("$selected");
        ParseIndexerSelectedObject(args, changeEventVal);
    } else if (!isArc && length > 0 && selectedProperty->IsObject()) {
        JSRef<JSObject> selectedObj = JSRef<JSObject>::Cast(selectedProperty);
        auto selectedValueProperty = selectedObj->GetProperty("value");
        if (selectedValueProperty->IsNumber()) {
            selectedVal = selectedValueProperty->ToNumber<int32_t>();
        }
        IndexerModel::GetInstance()->Create(indexerArray, selectedVal, isArc);
        JSIndexerTheme::ApplyTheme();
        JSRef<JSVal> changeEventVal = selectedObj->GetProperty("changeEvent");
        if (!changeEventVal.IsEmpty()) {
            if (!changeEventVal->IsUndefined() && changeEventVal->IsFunction()) {
                ParseIndexerSelectedObject(args, changeEventVal);
            }
            return;
        }

        args.ReturnSelf();
    } else {
        IndexerModel::GetInstance()->Create(indexerArray, selectedVal, isArc);
        JSIndexerTheme::ApplyTheme();
    }
}

void JSIndexer::SetSelectedColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    std::optional<Color> colorOpt;
    Color selectedColor;
    RefPtr<ResourceObject> resObj;
    if (ParseJsColor(args[0], selectedColor, resObj)) {
        colorOpt = selectedColor;
    }
    IndexerModel::GetInstance()->SetSelectedColor(colorOpt);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::SELECTED_COLOR, resObj);
    }
}

void JSIndexer::SetColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    std::optional<Color> colorOpt;
    Color color;
    RefPtr<ResourceObject> resObj;
    if (ParseJsColor(args[0], color, resObj)) {
        colorOpt = color;
    }
    IndexerModel::GetInstance()->SetColor(colorOpt);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::COLOR, resObj);
    }
}

void JSIndexer::SetPopupColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    std::optional<Color> colorOpt;
    Color popupColor;
    RefPtr<ResourceObject> resObj;
    if (ParseJsColor(args[0], popupColor, resObj)) {
        colorOpt = popupColor;
    }
    IndexerModel::GetInstance()->SetPopupColor(colorOpt);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::POPUP_COLOR, resObj);
    }
}

void JSIndexer::SetSelectedBackgroundColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    std::optional<Color> colorOpt;
    Color selectedBackgroundColor;
    RefPtr<ResourceObject> resObj;
    if (ParseJsColor(args[0], selectedBackgroundColor, resObj)) {
        colorOpt = selectedBackgroundColor;
    }
    IndexerModel::GetInstance()->SetSelectedBackgroundColor(colorOpt);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::SELECTED_BACKGROUND_COLOR, resObj);
    }
}

void JSIndexer::SetPopupBackground(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    std::optional<Color> colorOpt;
    Color popupBackground;
    RefPtr<ResourceObject> resObj;
    if (ParseJsColor(args[0], popupBackground, resObj)) {
        colorOpt = popupBackground;
    }
    IndexerModel::GetInstance()->SetPopupBackground(colorOpt);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::POPUP_BACKGROUND, resObj);
    }
}

void JSIndexer::SetUsingPopup(bool state)
{
    IndexerModel::GetInstance()->SetUsingPopup(state);
}

void JSIndexer::SetSelectedFont(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    IndexerTextStyle textStyle;
    if (args[0]->IsObject()) {
        GetFontContent(args, textStyle);
    }
    IndexerModel::GetInstance()->SetSelectedFont(
        textStyle.fontSize, textStyle.fontWeight, textStyle.fontFamily, textStyle.fontStyle);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(
            IndexerJsResourceType::SELECTED_FONT_SIZE, textStyle.fontSizeResObj);
        IndexerModel::GetInstance()->CreateWithResourceObj(
            IndexerJsResourceType::SELECTED_FONT_FAMILY, textStyle.fontFamilyResObj);
    }
}

void JSIndexer::SetPopupFont(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    IndexerTextStyle textStyle;
    if (args[0]->IsObject()) {
        GetFontContent(args, textStyle);
    }
    IndexerModel::GetInstance()->SetPopupFont(
        textStyle.fontSize, textStyle.fontWeight, textStyle.fontFamily, textStyle.fontStyle);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(
            IndexerJsResourceType::POPUP_FONT_SIZE, textStyle.fontSizeResObj);
        IndexerModel::GetInstance()->CreateWithResourceObj(
            IndexerJsResourceType::POPUP_FONT_FAMILY, textStyle.fontFamilyResObj);
    }
}

void JSIndexer::SetFont(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    IndexerTextStyle textStyle;
    if (args[0]->IsObject()) {
         GetFontContent(args, textStyle);
    }
    IndexerModel::GetInstance()->SetFont(
        textStyle.fontSize, textStyle.fontWeight, textStyle.fontFamily, textStyle.fontStyle);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::FONT_SIZE, textStyle.fontSizeResObj);
        IndexerModel::GetInstance()->CreateWithResourceObj(
            IndexerJsResourceType::FONT_FAMILY, textStyle.fontFamilyResObj);
    }
}

void JSIndexer::GetFontContent(const JSCallbackInfo& args, IndexerTextStyle& textStyle)
{
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    JSRef<JSVal> size = obj->GetProperty("size");
    CalcDimension fontSizeData;
    if (ParseJsDimensionFp(size, fontSizeData, textStyle.fontSizeResObj) && !fontSizeData.IsNegative() &&
        fontSizeData.Unit() != DimensionUnit::PERCENT) {
        textStyle.fontSize = fontSizeData;
    }

    JSRef<JSVal> weight = obj->GetProperty("weight");
    if (weight->IsString() || weight->IsNumber()) {
        textStyle.fontWeight = ConvertStrToFontWeight(weight->ToString());
    }

    JSRef<JSVal> family = obj->GetProperty("family");
    std::vector<std::string> fontFamilies;
    if (ParseJsFontFamilies(family, fontFamilies, textStyle.fontFamilyResObj)) {
        textStyle.fontFamily = fontFamilies;
    }

    JSRef<JSVal> style = obj->GetProperty("style");
    if (style->IsNumber()) {
        int32_t value = style->ToNumber<int32_t>();
        if (value >= 0 && value < static_cast<int32_t>(FONT_STYLES.size())) {
            textStyle.fontStyle = FONT_STYLES[value];
        }
    }
}

void JSIndexer::SetItemSize(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    CalcDimension itemSize;
    if (ParseJsDimensionVp(args[0], itemSize) && GreatNotEqual(itemSize.Value(), 0.0) &&
        itemSize.Unit() != DimensionUnit::PERCENT) {
        IndexerModel::GetInstance()->SetItemSize(itemSize);
        return;
    }
    IndexerModel::GetInstance()->SetItemSize(DEFAULT_ITEM_SIZE);
}

void JSIndexer::SetAlignStyle(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    int32_t value = Container::IsCurrentUseNewPipeline() ? static_cast<int32_t>(NG::AlignStyle::END)
                                                         : static_cast<int32_t>(V2::AlignStyle::END);
    auto alignValue = -1;
    if (args[0]->IsNumber()) {
        alignValue = args[0]->ToNumber<int32_t>();
    }
    if (alignValue >= 0 && alignValue < static_cast<int32_t>(ALIGN_STYLE.size())) {
        value = alignValue;
    }
    IndexerModel::GetInstance()->SetAlignStyle(value);
    CalcDimension popupHorizontalSpace(-1.0);
    if (args.Length() > 1) {
        RefPtr<ResourceObject> resObj;
        ParseJsDimensionVp(args[1], popupHorizontalSpace, resObj);
        if (SystemProperties::ConfigChangePerform()) {
            IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::ALIGN_OFFSET, resObj);
        }
    }
    IndexerModel::GetInstance()->SetPopupHorizontalSpace(popupHorizontalSpace);
}

void JSIndexer::SetSelected(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    int32_t selected = 0;
    auto selectedVal = args[0];
    JSRef<JSVal> changeEventVal;
    if (selectedVal->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(selectedVal);
        selectedVal = obj->GetProperty("value");
        changeEventVal = obj->GetProperty("$value");
    } else if (args.Length() > 1) {
        changeEventVal = args[1];
    }
    if (ParseJsInteger<int32_t>(selectedVal, selected)) {
        IndexerModel::GetInstance()->SetSelected(selected);
    }
    ParseIndexerSelectedObject(args, changeEventVal, true);
}

void JSIndexer::SetPopupPosition(const JSCallbackInfo& args)
{
    std::optional<Dimension> xOpt;
    std::optional<Dimension> yOpt;
    if (args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        CalcDimension x;
        CalcDimension y;
        RefPtr<ResourceObject> resObjX;
        RefPtr<ResourceObject> resObjY;
        JSRef<JSVal> xVal = obj->GetProperty("x");
        JSRef<JSVal> yVal = obj->GetProperty("y");
        if ((xVal->IsString() && StringUtils::StringToCalcDimensionNG(xVal->ToString(), x, false)) ||
            (!xVal->IsString() && JSViewAbstract::ParseJsDimensionVp(xVal, x, resObjX))) {
            xOpt = x;
        }
        if ((yVal->IsString() && StringUtils::StringToCalcDimensionNG(yVal->ToString(), y, false)) ||
            (!yVal->IsString() && JSViewAbstract::ParseJsDimensionVp(yVal, y, resObjY))) {
            yOpt = y;
        }
        if (SystemProperties::ConfigChangePerform()) {
            IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::POPUP_POSITION_X, resObjX);
            IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::POPUP_POSITION_Y, resObjY);
        }
    } else if (Container::LessThanAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        return;
    }
    IndexerModel::GetInstance()->SetPopupPositionX(xOpt);
    IndexerModel::GetInstance()->SetPopupPositionY(yOpt);
}

void JSIndexer::SetPopupSelectedColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    std::optional<Color> colorOpt;
    Color popupSelectedColor;
    RefPtr<ResourceObject> resObj;
    if (ParseJsColor(args[0], popupSelectedColor, resObj)) {
        colorOpt = popupSelectedColor;
    }
    IndexerModel::GetInstance()->SetPopupSelectedColor(colorOpt);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::POPUP_SELECTED_COLOR, resObj);
    }
}

void JSIndexer::SetPopupUnselectedColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    std::optional<Color> colorOpt;
    Color popupUnselectedColor;
    RefPtr<ResourceObject> resObj;
    if (ParseJsColor(args[0], popupUnselectedColor, resObj)) {
        colorOpt = popupUnselectedColor;
    }
    IndexerModel::GetInstance()->SetPopupUnselectedColor(colorOpt);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::POPUP_UNSELECTED_COLOR, resObj);
    }
}

void JSIndexer::SetPopupItemFont(const JSCallbackInfo& args)
{
    CalcDimension fontSize;
    std::string weight;
    RefPtr<ResourceObject> resObj;
    if (args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        JSRef<JSVal> size = obj->GetProperty("size");
        if (!size->IsNull()) {
            CalcDimension fontSizeData;
            if (ParseJsDimensionFp(size, fontSizeData, resObj) && !fontSizeData.IsNegative() &&
                fontSizeData.Unit() != DimensionUnit::PERCENT) {
                fontSize = fontSizeData;
            }
        }

        auto jsWeight = obj->GetProperty("weight");
        if (!jsWeight->IsNull()) {
            if (jsWeight->IsNumber()) {
                weight = std::to_string(jsWeight->ToNumber<int32_t>());
            } else {
                ParseJsString(jsWeight, weight);
            }
        }
    }
    IndexerModel::GetInstance()->SetFontSize(fontSize);
    IndexerModel::GetInstance()->SetFontWeight(ConvertStrToFontWeight(weight, FontWeight::MEDIUM));
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::POPUP_ITEM_FONT_SIZE, resObj);
    }
}

void JSIndexer::SetPopupItemBackgroundColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    std::optional<Color> colorOpt;
    Color popupItemBackgroundColor;
    RefPtr<ResourceObject> resObj;
    if (ParseJsColor(args[0], popupItemBackgroundColor, resObj)) {
        colorOpt = popupItemBackgroundColor;
    }
    IndexerModel::GetInstance()->SetPopupItemBackground(colorOpt);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::POPUP_ITEM_BACKGROUND_COLOR, resObj);
    }
}

std::optional<Color> JSIndexer::PaseColor(const JSCallbackInfo& args)
{
    std::optional<Color> colorOpt;
    Color color;
    if (ParseJsColor(args[0], color)) {
        colorOpt = color;
    }
    return colorOpt;
}

void JSIndexer::SetAutoCollapse(const JSCallbackInfo& args)
{
    bool state = true;
    if (args[0]->IsBoolean()) {
        state = args[0]->ToBoolean();
    }
    IndexerModel::GetInstance()->SetAutoCollapse(state);
}

void JSIndexer::SetPopupItemBorderRadius(const JSCallbackInfo& args)
{
    auto radius = Dimension(ZERO_RADIUS, DimensionUnit::VP);
    auto popupRadius = Dimension(ZERO_RADIUS, DimensionUnit::VP);
    if (args.Length() > 0 && args[0]->IsNumber()) {
        auto radiusValue = args[0]->ToNumber<double>();
        if (radiusValue >= 0) {
            radius.SetValue(radiusValue);
            radius.SetUnit(DimensionUnit::VP);
            popupRadius.SetValue(radiusValue + RADIUS_OFFSET);
            popupRadius.SetUnit(DimensionUnit::VP);
        }
    } else {
        radius.SetValue(POPUP_ITEM_DEFAULT_RADIUS);
        radius.SetUnit(DimensionUnit::VP);
        popupRadius.SetValue(radius.Value() + RADIUS_OFFSET);
        popupRadius.SetUnit(DimensionUnit::VP);
    }
    IndexerModel::GetInstance()->SetPopupItemBorderRadius(radius);
    IndexerModel::GetInstance()->SetPopupBorderRadius(popupRadius);
}

void JSIndexer::SetItemBorderRadius(const JSCallbackInfo& args)
{
    auto radius = Dimension(ZERO_RADIUS, DimensionUnit::VP);
    auto indexerRadius = Dimension(ZERO_RADIUS, DimensionUnit::VP);
    if (args.Length() > 0 && args[0]->IsNumber()) {
        auto radiusValue = args[0]->ToNumber<double>();
        if (radiusValue >= 0) {
            radius.SetValue(radiusValue);
            radius.SetUnit(DimensionUnit::VP);
            indexerRadius.SetValue(radiusValue + RADIUS_OFFSET);
            indexerRadius.SetUnit(DimensionUnit::VP);
        }
    } else {
        radius.SetValue(ITEM_DEFAULT_RADIUS);
        radius.SetUnit(DimensionUnit::VP);
        indexerRadius.SetValue(radius.Value() + RADIUS_OFFSET);
        indexerRadius.SetUnit(DimensionUnit::VP);
    }
    IndexerModel::GetInstance()->SetItemBorderRadius(radius);
    IndexerModel::GetInstance()->SetIndexerBorderRadius(indexerRadius);
}

void JSIndexer::SetPopupBackgroundBlurStyle(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }

    BlurStyleOption styleOption;
    if (args[0]->IsNumber()) {
        auto blurStyle = args[0]->ToNumber<int32_t>();
        if (blurStyle >= static_cast<int>(BlurStyle::NO_MATERIAL) &&
            blurStyle <= static_cast<int>(BlurStyle::COMPONENT_ULTRA_THICK)) {
            styleOption.blurStyle = static_cast<BlurStyle>(blurStyle);
        } else {
            styleOption.blurStyle = BlurStyle::COMPONENT_REGULAR;
        }
    } else {
        styleOption.blurStyle = BlurStyle::COMPONENT_REGULAR;
    }
    IndexerModel::GetInstance()->SetPopupBackgroundBlurStyle(styleOption);
}

void JSIndexer::SetPopupTitleBackground(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        return;
    }
    std::optional<Color> colorOpt;
    Color popupTitleBackground;
    RefPtr<ResourceObject> resObj;
    if (ParseJsColor(args[0], popupTitleBackground, resObj)) {
        colorOpt = popupTitleBackground;
    }
    IndexerModel::GetInstance()->SetPopupTitleBackground(colorOpt);
    if (SystemProperties::ConfigChangePerform()) {
        IndexerModel::GetInstance()->CreateWithResourceObj(IndexerJsResourceType::POPUP_TITLE_BACKGROUND, resObj);
    }
}

void JSIndexer::SetWidth(const JSCallbackInfo& args)
{
    JSViewAbstract::JsWidth(args);
    if (args[0]->IsString() && args[0]->ToString() == "auto") {
        IndexerModel::GetInstance()->SetAdaptiveWidth(true);
    } else {
        IndexerModel::GetInstance()->SetAdaptiveWidth(false);
    }
}

void JSIndexer::SetEnableHapticFeedback(const JSCallbackInfo& args)
{
    bool state = true;
    if (args.Length() > 0 && args[0]->IsBoolean()) {
        state = args[0]->ToBoolean();
    }
    IndexerModel::GetInstance()->SetEnableHapticFeedback(state);
}

void JSIndexer::JSBind(BindingTarget globalObj)
{
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSIndexer>::Declare("AlphabetIndexer");
    JSClass<JSIndexer>::StaticMethod("create", &JSIndexer::Create);
    JSClass<JSIndexer>::StaticMethod("createArc", &JSIndexer::CreateArc);
    JSClass<JSIndexer>::StaticMethod("color", &JSIndexer::SetColor, opt);
    JSClass<JSIndexer>::StaticMethod("selectedColor", &JSIndexer::SetSelectedColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupColor", &JSIndexer::SetPopupColor, opt);
    JSClass<JSIndexer>::StaticMethod("selectedBackgroundColor", &JSIndexer::SetSelectedBackgroundColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupBackground", &JSIndexer::SetPopupBackground, opt);
    JSClass<JSIndexer>::StaticMethod("usingPopup", &JSIndexer::SetUsingPopup, opt);
    JSClass<JSIndexer>::StaticMethod("selectedFont", &JSIndexer::SetSelectedFont);
    JSClass<JSIndexer>::StaticMethod("font", &JSIndexer::SetFont);
    JSClass<JSIndexer>::StaticMethod("popupFont", &JSIndexer::SetPopupFont);
    JSClass<JSIndexer>::StaticMethod("itemSize", &JSIndexer::SetItemSize, opt);
    JSClass<JSIndexer>::StaticMethod("alignStyle", &JSIndexer::SetAlignStyle, opt);
    JSClass<JSIndexer>::StaticMethod("selected", &JSIndexer::SetSelected, opt);
    JSClass<JSIndexer>::StaticMethod("popupPosition", &JSIndexer::SetPopupPosition, opt);
    JSClass<JSIndexer>::StaticMethod("popupSelectedColor", &JSIndexer::SetPopupSelectedColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupUnselectedColor", &JSIndexer::SetPopupUnselectedColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupItemFont", &JSIndexer::SetPopupItemFont);
    JSClass<JSIndexer>::StaticMethod("popupItemBackgroundColor", &JSIndexer::SetPopupItemBackgroundColor, opt);
    JSClass<JSIndexer>::StaticMethod("autoCollapse", &JSIndexer::SetAutoCollapse, opt);
    JSClass<JSIndexer>::StaticMethod("popupItemBorderRadius", &JSIndexer::SetPopupItemBorderRadius);
    JSClass<JSIndexer>::StaticMethod("itemBorderRadius", &JSIndexer::SetItemBorderRadius);
    JSClass<JSIndexer>::StaticMethod("popupBackgroundBlurStyle", &JSIndexer::SetPopupBackgroundBlurStyle);
    JSClass<JSIndexer>::StaticMethod("popupTitleBackground", &JSIndexer::SetPopupTitleBackground, opt);
    JSClass<JSIndexer>::StaticMethod("width", &JSIndexer::SetWidth);
    JSClass<JSIndexer>::StaticMethod("enableHapticFeedback", &JSIndexer::SetEnableHapticFeedback, opt);
    JSClass<JSIndexer>::StaticMethod("onAttach", &JSInteractableView::JsOnAttach);
    JSClass<JSIndexer>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSIndexer>::StaticMethod("onDetach", &JSInteractableView::JsOnDetach);
    JSClass<JSIndexer>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSIndexer>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSIndexer>::InheritAndBind<JSViewAbstract>(globalObj);
}
} // namespace OHOS::Ace::Framework
