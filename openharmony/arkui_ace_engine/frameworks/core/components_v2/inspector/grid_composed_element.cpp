/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components_v2/inspector/grid_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components_v2/grid/render_grid_scroll.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const GridComposedElement&)>> CREATE_JSON_MAP {
    { "columnsTemplate", [](const GridComposedElement& inspector) { return inspector.GetColumnsTemplate(); } },
    { "rowsTemplate", [](const GridComposedElement& inspector) { return inspector.GetRowsTemplate(); } },
    { "columnsGap", [](const GridComposedElement& inspector) { return inspector.GetColumnsGap(); } },
    { "rowsGap", [](const GridComposedElement& inspector) { return inspector.GetRowsGap(); } },
    { "scrollBarColor", [](const GridComposedElement& inspector) { return inspector.GetScrollBarColor(); } },
    { "scrollBarWidth", [](const GridComposedElement& inspector) { return inspector.GetScrollBarWidth(); } },
    { "scrollBar", [](const GridComposedElement& inspector) { return inspector.GetScrollBar(); } },
    { "editMode", [](const GridComposedElement& inspector) { return inspector.GetGridEditMode(); } },
    { "maxCount", [](const GridComposedElement& inspector) { return inspector.GetGridMaxCount(); } },
    { "minCount", [](const GridComposedElement& inspector) { return inspector.GetGridMinCount(); } },
    { "cellLength", [](const GridComposedElement& inspector) { return inspector.GetGridCellLength(); } },
    { "multiSelectable", [](const GridComposedElement& inspector) { return inspector.GetMultiSelectable(); } },
    { "supportAnimation", [](const GridComposedElement& inspector) { return inspector.GetSupportAnimation(); } },
};

}

void GridComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("columnsTemplate: ").append(GetColumnsTemplate()));
    DumpLog::GetInstance().AddDesc(
        std::string("rowsTemplate: ").append(GetRowsTemplate()));
    DumpLog::GetInstance().AddDesc(
        std::string("columnsGap: ").append(GetColumnsGap()));
    DumpLog::GetInstance().AddDesc(
        std::string("rowsGap: ").append(GetRowsGap()));
    DumpLog::GetInstance().AddDesc(
        std::string("editMode: ").append(GetGridEditMode()));
    DumpLog::GetInstance().AddDesc(
        std::string("maxCount: ").append(GetGridMaxCount()));
    DumpLog::GetInstance().AddDesc(
        std::string("minCount: ").append(GetGridMinCount()));
    DumpLog::GetInstance().AddDesc(
        std::string("cellLength: ").append(GetGridCellLength()));
}

std::unique_ptr<JsonValue> GridComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string GridComposedElement::GetColumnsTemplate() const
{
    auto node = GetInspectorNode(GridLayoutElement::TypeId());
    if (!node) {
        return "1fr";
    }
    auto renderGrip = AceType::DynamicCast<RenderGridLayout>(node);
    if (renderGrip) {
        return renderGrip->GetColumnsTemplate();
    }
    return "1fr";
}

std::string GridComposedElement::GetRowsTemplate() const
{
    auto node = GetInspectorNode(GridLayoutElement::TypeId());
    if (!node) {
        return "1fr";
    }
    auto renderGrip = AceType::DynamicCast<RenderGridLayout>(node);
    if (renderGrip) {
        return renderGrip->GetRowTemplate();
    }
    return "1fr";
}

std::string GridComposedElement::GetColumnsGap() const
{
    auto gridLayoutNode = GetInspectorNode(GridLayoutElement::TypeId());
    auto gridNode = GetInspectorNode(V2::GridElement::TypeId());
    if (!gridNode && !gridLayoutNode) {
        return "0";
    }
    auto renderGrip = AceType::DynamicCast<RenderGridLayout>(gridLayoutNode);
    auto renderScroll = AceType::DynamicCast<V2::RenderGridScroll>(gridNode);
    if (renderGrip) {
        return renderGrip->GetColumns().ToString().c_str();
    }
    if (renderScroll) {
        return renderScroll->GetColumns().ToString().c_str();
    }
    return "0";
}

std::string GridComposedElement::GetRowsGap() const
{
    auto gridLayoutNode = GetInspectorNode(GridLayoutElement::TypeId());
    auto gridNode = GetInspectorNode(V2::GridElement::TypeId());
    if (!gridNode && !gridLayoutNode) {
        return "0";
    }
    auto renderGrip = AceType::DynamicCast<RenderGridLayout>(gridLayoutNode);
    auto renderScroll = AceType::DynamicCast<V2::RenderGridScroll>(gridNode);
    if (renderGrip) {
        return renderGrip->GetRows().ToString().c_str();
    }
    if (renderScroll) {
        return renderScroll->GetRows().ToString().c_str();
    }
    return "0";
}

std::string GridComposedElement::GetScrollBarWidth() const
{
    auto node = GetInspectorNode(GridLayoutElement::TypeId());
    if (!node) {
        return "";
    }
    auto renderGrip = AceType::DynamicCast<RenderGridLayout>(node);
    if (renderGrip) {
        return renderGrip->GetScrollBarWidth();
    }
    return "";
}

std::string GridComposedElement::GetScrollBarColor() const
{
    auto node = GetInspectorNode(GridLayoutElement::TypeId());
    if (!node) {
        return "";
    }
    auto renderGrip = AceType::DynamicCast<RenderGridLayout>(node);
    if (renderGrip) {
        return renderGrip->GetScrollBarColor();
    }
    return "";
}

std::string GridComposedElement::GetScrollBar() const
{
    auto node = GetInspectorNode(GridLayoutElement::TypeId());
    if (!node) {
        return "BarState.Off";
    }
    auto renderGrip = AceType::DynamicCast<RenderGridLayout>(node);
    if (renderGrip) {
        return DisplayModeToString(renderGrip->GetScrollBar());
    }
    return "BarState.Off";
}

std::string GridComposedElement::GetGridEditMode() const
{
    auto renderGrip = GetContentRender<RenderGridLayout>(GridLayoutElement::TypeId());
    if (renderGrip) {
        return ConvertBoolToString(renderGrip->GetEditMode());
    }
    return "false";
}

std::string GridComposedElement::GetGridMaxCount() const
{
    auto renderGrip = GetContentRender<RenderGridLayout>(GridLayoutElement::TypeId());
    if (renderGrip) {
        return std::to_string(renderGrip->GetMaxCount());
    }
    return "1";
}

std::string GridComposedElement::GetGridMinCount() const
{
    auto renderGrip = GetContentRender<RenderGridLayout>(GridLayoutElement::TypeId());
    if (renderGrip) {
        return std::to_string(renderGrip->GetMinCount());
    }
    return "1";
}

std::string GridComposedElement::GetGridCellLength() const
{
    auto renderGrip = GetContentRender<RenderGridLayout>(GridLayoutElement::TypeId());
    if (renderGrip) {
        return std::to_string(renderGrip->GetCellLength());
    }
    return "0";
}

std::string GridComposedElement::GetSupportAnimation() const
{
    auto renderGrip = GetContentRender<RenderGridLayout>(GridLayoutElement::TypeId());
    if (renderGrip) {
        return ConvertBoolToString(renderGrip->GetSupportAnimation());
    }
    return "false";
}

std::string GridComposedElement::GetMultiSelectable() const
{
    auto renderGrip = GetContentRender<RenderGridLayout>(GridLayoutElement::TypeId());
    if (renderGrip) {
        return ConvertBoolToString(renderGrip->GetMultiSelectable());
    }
    return "false";
}

std::string GridComposedElement::DisplayModeToString(DisplayMode displayMode) const
{
    switch (displayMode) {
        case DisplayMode::OFF:
            return "BarState.Off";
        case DisplayMode::AUTO:
            return "BarState.Auto";
        case DisplayMode::ON:
            return "BarState.On";
        default:
            break;
    }
    return "BarState.Off";
}

} // namespace OHOS::Ace::V2