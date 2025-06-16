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

#include "bridge/declarative_frontend/jsview/js_grid.h"

#include "interfaces/inner_api/ui_session/ui_session_manager.h"

#include "base/log/ace_scoring_log.h"
#include "base/utils/utils.h"
#include "bridge/declarative_frontend/engine/functions/js_drag_function.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_scrollable.h"
#include "bridge/declarative_frontend/jsview/js_scroller.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/jsview/models/grid_model_impl.h"
#include "core/common/ace_application_info.h"
#include "core/common/container.h"
#include "core/components_ng/base/view_stack_model.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/grid/grid_model_ng.h"

namespace OHOS::Ace {

std::unique_ptr<GridModel> GridModel::instance_ = nullptr;
std::mutex GridModel::mutex_;

GridModel* GridModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::GridModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::GridModelNG());
            } else {
                instance_.reset(new Framework::GridModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}

} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {
namespace {
const std::vector<DisplayMode> DISPLAY_MODE = { DisplayMode::OFF, DisplayMode::AUTO, DisplayMode::ON };
const std::vector<EdgeEffect> EDGE_EFFECT = { EdgeEffect::SPRING, EdgeEffect::FADE, EdgeEffect::NONE };
const std::vector<FlexDirection> LAYOUT_DIRECTION = { FlexDirection::ROW, FlexDirection::COLUMN,
    FlexDirection::ROW_REVERSE, FlexDirection::COLUMN_REVERSE };
const size_t GRID_ITEM_SIZE_RESULT_LENGTH = 2;
const size_t GRID_ITEM_RECT_RESULT_LENGTH = 4;

void ParseGridItemSize(const panda::ecmascript::EcmaVM* vm, const Local<JSValueRef>& value, GridItemSize& gridItemSize)
{
    if (value->IsArray(vm)) {
        auto array = Local<panda::ArrayRef>(value);
        auto length = array->Length(vm);
        if (length != GRID_ITEM_SIZE_RESULT_LENGTH) {
            return;
        }
        auto rows = panda::ArrayRef::GetValueAt(vm, array, 0);
        if (rows->IsNumber()) {
            gridItemSize.rows = static_cast<int32_t>(rows->ToNumber(vm)->Value());
        }
        auto columns = panda::ArrayRef::GetValueAt(vm, array, 1);
        if (columns->IsNumber()) {
            gridItemSize.columns = static_cast<int32_t>(columns->ToNumber(vm)->Value());
        }
    }
}

void ParseGridItemRect(const panda::ecmascript::EcmaVM* vm, const Local<JSValueRef>& value, GridItemRect& gridItemRect)
{
    if (value->IsArray(vm)) {
        auto array = Local<panda::ArrayRef>(value);
        auto length = array->Length(vm);
        if (length != GRID_ITEM_RECT_RESULT_LENGTH) {
            return;
        }
        auto rowStart = panda::ArrayRef::GetValueAt(vm, array, GridItemRect::ROW_START);
        if (rowStart->IsNumber()) {
            gridItemRect.rowStart = static_cast<int32_t>(rowStart->ToNumber(vm)->Value());
        }
        auto rowSpan = panda::ArrayRef::GetValueAt(vm, array, GridItemRect::ROW_SPAN);
        if (rowSpan->IsNumber()) {
            gridItemRect.rowSpan = static_cast<int32_t>(rowSpan->ToNumber(vm)->Value());
        }
        auto columnStart = panda::ArrayRef::GetValueAt(vm, array, GridItemRect::COLUMN_START);
        if (columnStart->IsNumber()) {
            gridItemRect.columnStart = static_cast<int32_t>(columnStart->ToNumber(vm)->Value());
        }
        auto columnSpan = panda::ArrayRef::GetValueAt(vm, array, GridItemRect::COLUMN_SPAN);
        if (columnSpan->IsNumber()) {
            gridItemRect.columnSpan = static_cast<int32_t>(columnSpan->ToNumber(vm)->Value());
        }
    }
}

void ParseGetGridItemSize(const JSCallbackInfo& info, JSRef<JSObject>& obj, GridLayoutOptions& option)
{
    auto getSizeByIndex = obj->GetProperty("onGetIrregularSizeByIndex");
    if (getSizeByIndex->IsFunction()) {
        auto vm = info.GetVm();
        auto jsFunc = JSRef<JSFunc>::Cast(getSizeByIndex);
        auto func = jsFunc->GetLocalHandle();
        auto onGetIrregularSizeByIndex = [vm, func = panda::CopyableGlobal(vm, func)](int32_t index) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            GridItemSize gridItemSize;
            panda::Local<panda::JSValueRef> params[1] = { panda::NumberRef::New(vm, index) };
            auto result = func->Call(vm, func.ToLocal(), params, 1);
            if (!result->IsArray(vm)) {
                return gridItemSize;
            }
            ParseGridItemSize(vm, result, gridItemSize);
            return gridItemSize;
        };
        option.getSizeByIndex = std::move(onGetIrregularSizeByIndex);
    }
}

void ParseGetGridItemRect(const JSCallbackInfo& info, JSRef<JSObject>& obj, GridLayoutOptions& option)
{
    auto getRectByIndex = obj->GetProperty("onGetRectByIndex");
    if (getRectByIndex->IsFunction()) {
        auto vm = info.GetVm();
        auto jsFunc = JSRef<JSFunc>::Cast(getRectByIndex);
        auto func = jsFunc->GetLocalHandle();
        auto onGetRectByIndex = [vm, func = panda::CopyableGlobal(vm, func)](int32_t index) {
            panda::LocalScope pandaScope(vm);
            panda::TryCatch trycatch(vm);
            GridItemRect gridItemRect;
            panda::Local<panda::JSValueRef> params[1] = { panda::NumberRef::New(vm, index) };
            auto result = func->Call(vm, func.ToLocal(), params, 1);
            if (!result->IsArray(vm)) {
                return gridItemRect;
            }
            ParseGridItemRect(vm, result, gridItemRect);
            return gridItemRect;
        };
        option.getRectByIndex = std::move(onGetRectByIndex);
    }
}

void SetGridLayoutOptions(const JSCallbackInfo& info)
{
    if (!(info.Length() > 1 && info[1]->IsObject())) {
        return;
    }
    GridLayoutOptions option;
    auto obj = JSRef<JSObject>::Cast(info[1]);
    auto value = obj->GetProperty("regularSize");
    ParseGridItemSize(info.GetVm(), value->GetLocalHandle(), option.regularSize);

    // only support regularSize(1, 1)
    option.regularSize.rows = 1;
    option.regularSize.columns = 1;

    auto indexes = obj->GetProperty("irregularIndexes");
    if (indexes->IsArray()) {
        JSRef<JSArray> array = JSRef<JSArray>::Cast(indexes);
        auto length = array->Length();
        for (size_t i = 0; i < length; i++) {
            JSRef<JSVal> index = array->GetValueAt(i);
            if (!index->IsNumber()) {
                continue;
            }
            auto indexNum = index->ToNumber<int32_t>();
            if (indexNum >= 0) {
                option.irregularIndexes.emplace(indexNum);
            }
        }
    }

    ParseGetGridItemSize(info, obj, option);
    ParseGetGridItemRect(info, obj, option);

    GridModel::GetInstance()->SetLayoutOptions(option);
}
} // namespace

void JSGrid::Create(const JSCallbackInfo& info)
{
    RefPtr<ScrollControllerBase> positionController;
    RefPtr<ScrollProxy> scrollBarProxy;
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSScroller* jsScroller = JSRef<JSObject>::Cast(info[0])->Unwrap<JSScroller>();
        if (jsScroller) {
            jsScroller->SetInstanceId(Container::CurrentId());
            positionController = GridModel::GetInstance()->CreatePositionController();
            jsScroller->SetController(positionController);

            // Init scroll bar proxy.
            scrollBarProxy = jsScroller->GetScrollBarProxy();
            if (!scrollBarProxy) {
                scrollBarProxy = GridModel::GetInstance()->CreateScrollBarProxy();
                jsScroller->SetScrollBarProxy(scrollBarProxy);
            }
        }
    }
    GridModel::GetInstance()->Create(positionController, scrollBarProxy);

    SetGridLayoutOptions(info);
}

void JSGrid::PopGrid()
{
    if (ViewStackModel::GetInstance()->IsPrebuilding()) {
        return ViewStackModel::GetInstance()->PushPrebuildCompCmd("[JSGrid][pop]", &JSGrid::PopGrid);
    }
    GridModel::GetInstance()->Pop();
}

void JSGrid::UseProxy(const JSCallbackInfo& args)
{
#ifdef NG_BUILD
    args.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(false)));
#else
    auto parentGrid = ViewStackProcessor::GetInstance()->GetTopGrid();

    // return true if code path for GridElement and its children will rely on
    // ElementProxy. Only in this case shallow render functionality can be used
    // see also GridLayoutComponent::CreateElement() and GridItemElementProxy class
    args.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(parentGrid ? !parentGrid->UseNonProxiedCodePath() : false)));
#endif
}

void JSGrid::SetColumnsTemplate(const std::string& value)
{
    GridModel::GetInstance()->SetColumnsTemplate(value);
}

void JSGrid::SetRowsTemplate(const std::string& value)
{
    GridModel::GetInstance()->SetRowsTemplate(value);
}

void JSGrid::SetColumnsGap(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    CalcDimension colGap;

    if (!ParseJsDimensionVp(info[0], colGap) || colGap.Value() < 0) {
        colGap.SetValue(0.0);
    }

    GridModel::GetInstance()->SetColumnsGap(colGap);
}

void JSGrid::SetRowsGap(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }
    CalcDimension rowGap;

    if (!ParseJsDimensionVp(info[0], rowGap) || rowGap.Value() < 0) {
        rowGap.SetValue(0.0);
    }

    GridModel::GetInstance()->SetRowsGap(rowGap);
}

void JSGrid::JsGridHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        return;
    }

    CalcDimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }
    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    GridModel::GetInstance()->SetGridHeight(value);
}

void JSGrid::SetScrollEnabled(const JSCallbackInfo& args)
{
    GridModel::GetInstance()->SetScrollEnabled(args[0]->IsBoolean() ? args[0]->ToBoolean() : true);
}

void JSGrid::JSBind(BindingTarget globalObj)
{
    JSClass<JSGrid>::Declare("Grid");

    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSGrid>::StaticMethod("create", &JSGrid::Create, opt);
    JSClass<JSGrid>::StaticMethod("pop", &JSGrid::PopGrid, opt);
    JSClass<JSGrid>::StaticMethod("willUseProxy", &JSGrid::UseProxy, opt);
    JSClass<JSGrid>::StaticMethod("columnsTemplate", &JSGrid::SetColumnsTemplate, opt);
    JSClass<JSGrid>::StaticMethod("rowsTemplate", &JSGrid::SetRowsTemplate, opt);
    JSClass<JSGrid>::StaticMethod("columnsGap", &JSGrid::SetColumnsGap, opt);
    JSClass<JSGrid>::StaticMethod("rowsGap", &JSGrid::SetRowsGap, opt);
    JSClass<JSGrid>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSGrid>::StaticMethod("onAttach", &JSInteractableView::JsOnAttach);
    JSClass<JSGrid>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSGrid>::StaticMethod("onDetach", &JSInteractableView::JsOnDetach);
    JSClass<JSGrid>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSGrid>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSGrid>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSGrid>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSGrid>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSGrid>::StaticMethod("scrollBar", &JSGrid::SetScrollBar, opt);
    JSClass<JSGrid>::StaticMethod("scrollBarWidth", &JSGrid::SetScrollBarWidth, opt);
    JSClass<JSGrid>::StaticMethod("scrollBarColor", &JSGrid::SetScrollBarColor, opt);
    JSClass<JSGrid>::StaticMethod("clip", &JSScrollable::JsClip);

    JSClass<JSGrid>::StaticMethod("cachedCount", &JSGrid::SetCachedCount);
    JSClass<JSGrid>::StaticMethod("editMode", &JSGrid::SetEditMode, opt);
    JSClass<JSGrid>::StaticMethod("multiSelectable", &JSGrid::SetMultiSelectable, opt);
    JSClass<JSGrid>::StaticMethod("maxCount", &JSGrid::SetMaxCount, opt);
    JSClass<JSGrid>::StaticMethod("minCount", &JSGrid::SetMinCount, opt);
    JSClass<JSGrid>::StaticMethod("cellLength", &JSGrid::CellLength, opt);
    JSClass<JSGrid>::StaticMethod("layoutDirection", &JSGrid::SetLayoutDirection, opt);
    JSClass<JSGrid>::StaticMethod("dragAnimation", &JSGrid::SetDragAnimation, opt);
    JSClass<JSGrid>::StaticMethod("edgeEffect", &JSGrid::SetEdgeEffect, opt);
    JSClass<JSGrid>::StaticMethod("direction", &JSGrid::SetDirection, opt);
    JSClass<JSGrid>::StaticMethod("supportAnimation", &JSGrid::SetSupportAnimation, opt);
    JSClass<JSGrid>::StaticMethod("height", &JSGrid::JsGridHeight);
    JSClass<JSGrid>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSGrid>::StaticMethod("nestedScroll", &JSGrid::SetNestedScroll);
    JSClass<JSGrid>::StaticMethod("enableScrollInteraction", &JSGrid::SetScrollEnabled);
    JSClass<JSGrid>::StaticMethod("friction", &JSGrid::SetFriction);
    JSClass<JSGrid>::StaticMethod("focusWrapMode", &JSGrid::SetFocusWrapMode);
    JSClass<JSGrid>::StaticMethod("alignItems", &JSGrid::SetAlignItems);

    JSClass<JSGrid>::StaticMethod("onScroll", &JSGrid::JsOnScroll);

    JSClass<JSGrid>::InheritAndBind<JSScrollableBase>(globalObj);
}

void JSGrid::SetScrollBar(const JSCallbackInfo& info)
{
    auto displayMode = JSScrollable::ParseDisplayMode(info, GridModel::GetInstance()->GetDisplayMode());
    GridModel::GetInstance()->SetScrollBarMode(displayMode);
}

void JSGrid::SetScrollBarColor(const JSCallbackInfo& info)
{
    auto scrollBarColor = JSScrollable::ParseBarColor(info);
    if (!scrollBarColor.empty()) {
        GridModel::GetInstance()->SetScrollBarColor(scrollBarColor);
    }
}

void JSGrid::SetScrollBarWidth(const JSCallbackInfo& scrollWidth)
{
    auto scrollBarWidth = JSScrollable::ParseBarWidth(scrollWidth);
    if (!scrollBarWidth.empty()) {
        GridModel::GetInstance()->SetScrollBarWidth(scrollBarWidth);
    }
}

void JSGrid::SetCachedCount(const JSCallbackInfo& info)
{
    int32_t cachedCount = 1;
    auto jsValue = info[0];

    if (!jsValue->IsUndefined() && jsValue->IsNumber()) {
        cachedCount = jsValue->ToNumber<int32_t>();
        if (cachedCount < 0) {
            cachedCount = 1;
        }
    }
    bool show = false;
    if (info.Length() > 1) {
        show = info[1]->ToBoolean();
    }
    GridModel::GetInstance()->SetCachedCount(cachedCount, show);
}

void JSGrid::SetEditMode(const JSCallbackInfo& info)
{
    // undefined means false to EditMode
    bool editMode = false;
    if (!info[0]->IsUndefined() && info[0]->IsBoolean()) {
        ParseJsBool(info[0], editMode);
    }
    GridModel::GetInstance()->SetEditable(editMode);
}

void JSGrid::SetMaxCount(const JSCallbackInfo& info)
{
    int32_t maxCount = Infinity<int32_t>();
    if (!info[0]->IsUndefined() && info[0]->IsNumber()) {
        ParseJsInt32(info[0], maxCount);
        if (maxCount < 1) {
            maxCount = Infinity<int32_t>();
        }
    }
    GridModel::GetInstance()->SetMaxCount(maxCount);
}

void JSGrid::SetMinCount(const JSCallbackInfo& info)
{
    int32_t minCount = 1;
    if (!info[0]->IsUndefined() && info[0]->IsNumber()) {
        ParseJsInt32(info[0], minCount);
        if (minCount < 1) {
            minCount = 1;
        }
    }
    GridModel::GetInstance()->SetMinCount(minCount);
}

void JSGrid::CellLength(int32_t cellLength)
{
    GridModel::GetInstance()->SetCellLength(cellLength);
}

void JSGrid::SetSupportAnimation(bool supportAnimation)
{
    GridModel::GetInstance()->SetSupportAnimation(supportAnimation);
}

void JSGrid::SetDragAnimation(bool value)
{
    GridModel::GetInstance()->SetSupportDragAnimation(value);
}

void JSGrid::SetEdgeEffect(const JSCallbackInfo& info)
{
    auto edgeEffect = EdgeEffect::NONE;
    auto effectEdge = EffectEdge::ALL;
    if (info.Length() > 0) {
        edgeEffect = JSScrollable::ParseEdgeEffect(info[0], EdgeEffect::NONE);
    }
    auto alwaysEnabled = false;
    if (info.Length() > 1) {
        alwaysEnabled = JSScrollable::ParseAlwaysEnable(info[1], false);
        effectEdge = JSScrollable::ParseEffectEdge(info[1]);
    }
    GridModel::GetInstance()->SetEdgeEffect(edgeEffect, alwaysEnabled, effectEdge);
}

void JSGrid::SetLayoutDirection(int32_t value)
{
    if (value < 0 || value >= static_cast<int32_t>(LAYOUT_DIRECTION.size())) {
        return;
    }
    GridModel::GetInstance()->SetLayoutDirection(LAYOUT_DIRECTION[value]);
}

void JSGrid::SetDirection(const std::string& dir)
{
    TextDirection direction;
    if (dir == "Ltr") {
        direction = TextDirection::LTR;
    } else if (dir == "Rtl") {
        direction = TextDirection::RTL;
    } else {
        direction = TextDirection::AUTO;
    }
    GridModel::GetInstance()->SetIsRTL(direction);
}

void JSGrid::SetMultiSelectable(bool multiSelectable)
{
    GridModel::GetInstance()->SetMultiSelectable(multiSelectable);
}

void JSGrid::SetNestedScroll(const JSCallbackInfo& args)
{
    NestedScrollOptions nestedOpt = {
        .forward = NestedScrollMode::SELF_ONLY,
        .backward = NestedScrollMode::SELF_ONLY,
    };
    if (args.Length() < 1 || !args[0]->IsObject()) {
        GridModel::GetInstance()->SetNestedScroll(nestedOpt);
        return;
    }
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    int32_t froward = 0;
    JSViewAbstract::ParseJsInt32(obj->GetProperty("scrollForward"), froward);
    if (froward < static_cast<int32_t>(NestedScrollMode::SELF_ONLY) ||
        froward > static_cast<int32_t>(NestedScrollMode::PARALLEL)) {
        froward = 0;
    }
    int32_t backward = 0;
    JSViewAbstract::ParseJsInt32(obj->GetProperty("scrollBackward"), backward);
    if (backward < static_cast<int32_t>(NestedScrollMode::SELF_ONLY) ||
        backward > static_cast<int32_t>(NestedScrollMode::PARALLEL)) {
        backward = 0;
    }
    nestedOpt.forward = static_cast<NestedScrollMode>(froward);
    nestedOpt.backward = static_cast<NestedScrollMode>(backward);
    GridModel::GetInstance()->SetNestedScroll(nestedOpt);
    args.ReturnSelf();
}

void JSGrid::SetFriction(const JSCallbackInfo& info)
{
    double friction = -1.0;
    RefPtr<ResourceObject> resObj;
    if (!JSViewAbstract::ParseJsDouble(info[0], friction, resObj)) {
        friction = -1.0;
    }
    if (SystemProperties::ConfigChangePerform()) {
        GridModel::GetInstance()->CreateWithResourceObjFriction(resObj);
    }
    GridModel::GetInstance()->SetFriction(friction);
}

void JSGrid::SetFocusWrapMode(const JSCallbackInfo& args)
{
    auto focusWrapMode = static_cast<int32_t>(FocusWrapMode::DEFAULT);
    if (!JSViewAbstract::ParseJsInt32(args[0], focusWrapMode) ||
        focusWrapMode < static_cast<int32_t>(FocusWrapMode::DEFAULT) ||
        focusWrapMode > static_cast<int32_t>(FocusWrapMode::WRAP_WITH_ARROW)) {
        focusWrapMode = static_cast<int32_t>(FocusWrapMode::DEFAULT);
    }
    GridModel::GetInstance()->SetFocusWrapMode(static_cast<FocusWrapMode>(focusWrapMode));
}

void JSGrid::SetAlignItems(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        GridModel::GetInstance()->SetAlignItems(GridItemAlignment::DEFAULT);
        return;
    }

    if (info[0]->IsNumber()) {
        auto itemAlign = static_cast<GridItemAlignment>(info[0]->ToNumber<int32_t>());
        if (itemAlign < GridItemAlignment::DEFAULT || itemAlign > GridItemAlignment::STRETCH) {
            itemAlign = GridItemAlignment::DEFAULT;
        }
        GridModel::GetInstance()->SetAlignItems(itemAlign);
    } else {
        GridModel::GetInstance()->SetAlignItems(GridItemAlignment::DEFAULT);
    }
}

void JSGrid::JsOnScroll(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onScroll = [func = JSRef<JSFunc>::Cast(args[0])](
                            const CalcDimension& scrollOffset, const ScrollState& scrollState) {
            auto params = ConvertToJSValues(scrollOffset, scrollState);
            func->Call(JSRef<JSObject>(), params.size(), params.data());
            return;
        };
        GridModel::GetInstance()->SetOnScroll(std::move(onScroll));
    }
    args.ReturnSelf();
}
} // namespace OHOS::Ace::Framework
