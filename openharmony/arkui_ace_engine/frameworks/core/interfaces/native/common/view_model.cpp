/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "base/memory/ace_type.h"
#include "core/components_ng/base/group_node.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/pattern/badge/badge_model_ng.h"
#include "core/components_ng/pattern/calendar/calendar_model_ng.h"
#include "core/components_ng/pattern/calendar_picker/calendar_picker_model_ng.h"
#include "core/components_ng/pattern/common_view/common_view_model_ng.h"
#include "core/components_ng/pattern/canvas/canvas_model_ng.h"
#include "core/components_ng/pattern/data_panel/data_panel_model_ng.h"
#include "core/components_ng/pattern/folder_stack/folder_stack_model_ng.h"
#include "core/components_ng/pattern/form_link/form_link_model_ng.h"
#ifdef FORM_SUPPORTED
#include "core/components_ng/pattern/form/form_model_ng.h"
#endif
#include "core/components_ng/pattern/gauge/gauge_model_ng.h"
#include "core/components_ng/pattern/hyperlink/hyperlink_model_ng.h"
#include "core/components_ng/pattern/linear_layout/column_model_ng.h"
#include "core/components_ng/pattern/linear_layout/row_model_ng.h"
#include "core/components_ng/pattern/list/list_model_ng.h"
#include "core/components_ng/pattern/list/list_item_model_ng.h"
#include "core/components_ng/pattern/list/list_item_group_model_ng.h"
#include "core/components_ng/pattern/marquee/marquee_model_ng.h"
#include "core/components_ng/pattern/menu/menu_item/menu_item_model_ng.h"
#include "core/components_ng/pattern/menu/menu_model_ng.h"
#include "core/components_ng/pattern/menu/menu_item_group/menu_item_group_view.h"
#ifdef MODEL_COMPONENT_SUPPORTED
#include "core/components_ng/pattern/model/model_view_ng.h"
#endif //MODEL_COMPONENT_SUPPORTED
#include "core/components_ng/pattern/picker/datepicker_model_ng.h"
#include "core/components_ng/pattern/qrcode/qrcode_model_ng.h"
#include "core/components_ng/pattern/rating/rating_model_ng.h"
#include "core/components_ng/pattern/rich_editor/rich_editor_model_ng.h"
#include "core/components_ng/pattern/scroll/scroll_model_ng.h"
#include "core/components_ng/pattern/scroll_bar/scroll_bar_model_ng.h"
#include "core/components_ng/pattern/shape/circle_model_ng.h"
#include "core/components_ng/pattern/effect_component/effect_component_model_ng.h"
#include "core/components_ng/pattern/shape/ellipse_model_ng.h"
#include "core/components_ng/pattern/shape/path_model_ng.h"
#include "core/components_ng/pattern/shape/shape_model_ng.h"
#include "core/components_ng/pattern/stack/stack_model_ng.h"
#include "core/components_ng/pattern/stepper/stepper_item_model_ng.h"
#include "core/components_ng/pattern/stepper/stepper_model_ng.h"
#include "core/components_ng/pattern/tabs/tab_content_model_ng.h"
#include "core/components_ng/pattern/tabs/tabs_model_ng.h"
#include "core/components_ng/pattern/text/span/span_object.h"
#include "core/components_ng/pattern/text_clock/text_clock_model_ng.h"
#include "core/components_ng/pattern/text_field/text_field_model_ng.h"
#include "core/components_ng/pattern/text/image_span_view.h"
#include "core/components_ng/pattern/text/text_model_ng.h"
#include "core/components_ng/pattern/text/span_model_ng.h"
#include "core/components_ng/pattern/symbol/symbol_model_ng.h"
#include "core/components_ng/pattern/text/symbol_span_model_ng.h"
#include "core/components_ng/pattern/text_picker/textpicker_model_ng.h"
#include "core/components_ng/pattern/texttimer/text_timer_model_ng.h"
#include "core/components_ng/pattern/time_picker/timepicker_model_ng.h"
#include "core/components_ng/pattern/toggle/toggle_model_ng.h"
#ifdef WINDOW_SCENE_SUPPORTED
#include "core/components_ng/pattern/ui_extension/ui_extension_model_ng.h"
#endif //WINDOW_SCENE_SUPPORTED
#include "core/components_ng/pattern/image/image_model_ng.h"
#include "core/components_ng/pattern/list/list_model_ng.h"
#include "core/components_ng/pattern/loading_progress/loading_progress_model_ng.h"
#include "core/components_ng/pattern/swiper/swiper_model_ng.h"
#include "core/components_ng/pattern/button/button_model_ng.h"
#include "core/components_ng/pattern/patternlock/patternlock_model_ng.h"
#include "core/components_ng/pattern/progress/progress_model_ng.h"
#include "core/components_ng/pattern/checkbox/checkbox_model_ng.h"
#include "core/components_ng/pattern/checkboxgroup/checkboxgroup_model_ng.h"
#include "core/components_ng/pattern/linear_layout/column_model_ng.h"
#include "core/components_ng/pattern/linear_layout/row_model_ng.h"
#include "core/components_ng/pattern/navigator/navigator_model_ng.h"
#include "core/components_ng/pattern/navrouter/navdestination_model_ng.h"
#include "core/components_ng/pattern/navrouter/navrouter_model_ng.h"
#include "core/components_ng/pattern/flex/flex_model_ng.h"
#include "core/components_ng/pattern/refresh/refresh_model_ng.h"
#include "core/components_ng/pattern/shape/line_model_ng.h"
#include "core/components_ng/pattern/shape/polygon_model_ng.h"
#include "core/components_ng/pattern/shape/rect_model_ng.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_model_ng.h"
#include "core/components_ng/pattern/side_bar/side_bar_container_model_ng.h"
#include "core/components_ng/pattern/slider/slider_model_ng.h"
#include "core/components_ng/pattern/waterflow/water_flow_model_ng.h"
#include "core/components_ng/pattern/waterflow/water_flow_item_model_ng.h"
#include "core/components_ng/pattern/relative_container/relative_container_model_ng.h"
#include "core/components_ng/pattern/grid/grid_model_ng.h"
#include "core/components_ng/pattern/grid/grid_item_model_ng.h"
#include "core/components_ng/pattern/grid_col/grid_col_model_ng.h"
#include "core/components_ng/pattern/grid_row/grid_row_model_ng.h"
#include "core/components_ng/pattern/blank/blank_model_ng.h"
#include "core/components_ng/pattern/custom_frame_node/custom_pattern.h"
#include "core/components_ng/pattern/divider/divider_model_ng.h"
#include "core/components_ng/pattern/indexer/indexer_model_ng.h"
#include "core/components_ng/pattern/search/search_model_ng.h"
#include "core/components_ng/pattern/security_component/location_button/location_button_model_ng.h"
#include "core/components_ng/pattern/security_component/paste_button/paste_button_model_ng.h"
#include "core/components_ng/pattern/security_component/save_button/save_button_model_ng.h"
#include "core/components_ng/pattern/radio/radio_model_ng.h"
#include "core/components_ng/pattern/navigation/navigation_model_ng.h"
#include "core/components_ng/pattern/image_animator/image_animator_model_ng.h"
#include "core/components_ng/pattern/counter/counter_model_ng.h"
#include "core/components_ng/pattern/qrcode/qrcode_model_ng.h"
#include "core/components_ng/pattern/video/video_model_ng.h"
#ifdef WEB_SUPPORTED
#include "core/components_ng/pattern/web/richtext_model_ng.h"
#include "core/components_ng/pattern/web/web_model_ng.h"
#endif // WEB_SUPPORTED
#ifdef WINDOW_SCENE_SUPPORTED
#include "core/components_ng/pattern/window_scene/root/root_scene_model.h"
#include "core/components_ng/pattern/window_scene/screen/screen_model.h"
#endif // WINDOW_SCENE_SUPPORTED
#include "core/interfaces/native/node/node_api.h"
#include "core/interfaces/native/node/extension_companion_node.h"
#include "core/pipeline/base/element_register.h"
#ifdef PLUGIN_COMPONENT_SUPPORTED
#include "core/components_ng/pattern/plugin/plugin_model_ng.h"
#endif

namespace OHOS::Ace::NG::GeneratedViewModel {

ArkUIAPICallbackMethod* callbacks = nullptr;

void* createTextNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = TextModelNG::CreateFrameNode(nodeId, "");
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createSymbolNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SymbolModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createSpanNode(ArkUI_Int32 nodeId)
{
    // auto spanNode = SpanModelNG::CreateSpanNode(nodeId, "");
    // CHECK_NULL_RETURN(spanNode, nullptr);
    // spanNode->IncRefCount();
    // return AceType::RawPtr(spanNode);
    return nullptr;
}

void* createImageSpanNode(ArkUI_Int32 nodeId)
{
    // auto imageSpanNode = ImageSpanView::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(imageSpanNode, nullptr);
    // imageSpanNode->IncRefCount();
    // return AceType::RawPtr(imageSpanNode);
    return nullptr;
}

void* createImageNode(ArkUI_Int32 nodeId)
{
    // RefPtr<PixelMap> pixmap = nullptr;
    // auto frameNode = ImageModelNG::CreateFrameNode(nodeId, "", pixmap, "", "", false);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createToggleNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ToggleModelNG::CreateFrameNode(nodeId, NG::ToggleType::CHECKBOX, false);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createLoadingProgressNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = LoadingProgressModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createTextInputNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = TextFieldModelNG::CreateFrameNode(nodeId, "", "", false);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createStackNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = StackModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createScrollNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ScrollModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createListNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ListModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createSwiperNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SwiperModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createTextAreaNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = TextFieldModelNG::CreateFrameNode(nodeId, "", "", true);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createButtonNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ButtonModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createProgressNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ProgressModelNG::CreateFrameNode(nodeId, 0, 100, NG::ProgressType::LINEAR);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createCheckboxNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = CheckBoxModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createColumnNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ColumnModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRowNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = RowModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createFlexNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = FlexModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createListItemNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ListItemModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRefreshNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = RefreshModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRootNode(ArkUI_Int32 nodeId)
{
    auto container = Container::Current();
    CHECK_NULL_RETURN(container, nullptr);
    RefPtr<PipelineBase> pipeline;
    pipeline = container->GetPipelineContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto context = AceType::DynamicCast<NG::PipelineContext>(pipeline);
    CHECK_NULL_RETURN(context, nullptr);
    auto stageManager = context->GetStageManager();
    CHECK_NULL_RETURN(stageManager, nullptr);
    auto stageNode = stageManager->GetStageNode();
    TAG_LOGD(AceLogTag::ACE_NATIVE_NODE, "createRootNode: stageNode %{public}p", AceType::RawPtr(stageNode));
    return AceType::RawPtr(stageNode);
}

void* createComponentRootNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = CommonViewModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->GetLayoutProperty()->UpdateAlignment(Alignment::TOP_LEFT);
    // frameNode->IncRefCount();
    // TAG_LOGD(AceLogTag::ACE_NATIVE_NODE, "createComponentRootNode: frameNode %{public}p", AceType::RawPtr(frameNode));
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createXComponentNode(ArkUI_Int32 nodeId)
{
#ifdef XCOMPONENT_SUPPORTED
    // auto frameNode = XComponentModelNG::CreateFrameNode(nodeId, std::nullopt, XComponentType::SURFACE, std::nullopt);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
#else
    return nullptr;
#endif //XCOMPONENT_SUPPORTED
}

void* createListItemGroupNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ListItemGroupModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createSliderNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SliderModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createCanvasNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = CanvasModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createDatePickerNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = DatePickerModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createTimePickerNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = TimePickerModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createTextPickerNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = TextPickerModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createCalendarPickerNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = CalendarPickerModelNG::CreateEmptyFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createCustomNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = FrameNode::CreateFrameNode("Custom", nodeId, AceType::MakeRefPtr<CustomPattern>());
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createNavigationNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = NavigationModelNG::CreateFrameNode(nodeId);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createWaterFlowNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = WaterFlowModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createFlowItemNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = WaterFlowItemModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createCircleNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = CircleModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRelativeContainerNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = RelativeContainerModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}
void* createGridNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = GridModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createTabsNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = TabsModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createGridItemNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = GridItemModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createBlankNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = BlankModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createDividerNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = DividerModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createAlphabetIndexerNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = IndexerModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createSearchNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SearchModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createGridRowNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = GridRowModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createGridColNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = GridColModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createImageAnimatorNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ImageAnimatorModelNG::CreateFrameNode(nodeId);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRadioNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = RadioModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createSelectNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SelectModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createTabContentNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = TabContentModelNG::CreateFrameNode(nodeId);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createCustomSpanNode(ArkUI_Int32 nodeId)
{
    // auto customSpanNode = CustomSpanNode::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(customSpanNode, nullptr);
    // customSpanNode->IncRefCount();
    // return AceType::RawPtr(customSpanNode);
    return nullptr;
}

void* createQRCodeNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = QRCodeModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createBadgeNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = BadgeModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createTextClockNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = TextClockModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createTextTimerNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = TextTimerModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createMarqueeNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = MarqueeModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createCheckboxGroupNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = CheckBoxGroupModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRatingNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = RatingModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createAbilityComponentNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createAnimatorNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createCalendarNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = CalendarModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createCommonMethodNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createCommonShapeMethodNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createCommonNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = CommonViewModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->GetLayoutProperty()->UpdateAlignment(Alignment::TOP_LEFT);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createScrollableCommonMethodNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createComponent3DNode(ArkUI_Int32 nodeId)
{
#ifdef MODEL_COMPONENT_SUPPORTED
    // auto frameNode = ModelViewNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
#else
    return nullptr;
#endif // MODEL_COMPONENT_SUPPORTED
}

void* createContainerSpanNode(ArkUI_Int32 nodeId)
{
    // auto spanNode = SpanModelNG::CreateContainerSpanNode(nodeId);
    // CHECK_NULL_RETURN(spanNode, nullptr);
    // spanNode->IncRefCount();
    // return AceType::RawPtr(spanNode);
    return nullptr;
}

void* createCounterNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = CounterModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createDataPanelNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = DataPanelModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createEffectComponentNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = EffectComponentModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createEllipseNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = EllipseModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createEmbeddedComponentNode(ArkUI_Int32 nodeId)
{
#ifdef WINDOW_SCENE_SUPPORTED
    // auto frameNode = UIExtensionModelNG::CreateEmbeddedFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
#else
    return nullptr;
#endif // WINDOW_SCENE_SUPPORTED
}

void* createFolderStackNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = FolderStackModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createFormComponentNode(ArkUI_Int32 nodeId)
{
#ifdef FORM_SUPPORTED
    // auto frameNode = FormModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
#else
    return nullptr;
#endif
}

void* createFormLinkNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = FormLinkModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createGaugeNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = GaugeModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createGridContainerNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createHyperlinkNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = HyperlinkModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createLineNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = LineModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createLocationButtonNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = LocationButtonModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createMediaCachedImageNode(ArkUI_Int32 nodeId)
{
    // return createImageNode(nodeId);
    return nullptr;
}

void* createMenuNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = MenuModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createMenuItemNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = MenuItemModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createMenuItemGroupNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = MenuItemGroupView::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createNavDestinationNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = NavDestinationModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createNavRouterNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = NavRouterModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createNavigatorNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = NavigatorModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createNodeContainerNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createPasteButtonNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = PasteButtonModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createPathNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = PathModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createPatternLockNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = PatternLockModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createPluginComponentNode(ArkUI_Int32 nodeId)
{
#ifdef PLUGIN_COMPONENT_SUPPORTED
    // auto frameNode = PluginModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
#else
    return nullptr;
#endif
}

void* createPolygonNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = PolygonModelNG::CreateFrameNode(nodeId, true);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createPolylineNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = PolygonModelNG::CreateFrameNode(nodeId, false);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRectNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = RectModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRichEditorNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = RichEditorModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRichTextNode(ArkUI_Int32 nodeId)
{
#ifdef WEB_SUPPORTED
    // auto frameNode = RichTextModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
#else
    return nullptr;
#endif // WEB_SUPPORTED
}

void* createRootSceneNode(ArkUI_Int32 nodeId)
{
#ifdef WINDOW_SCENE_SUPPORTED
    // auto rootSceneNode = RootSceneModel::CreateRootSceneNode(nodeId);
    // rootSceneNode->IncRefCount();
    // return AceType::RawPtr(rootSceneNode);
    return nullptr;
#else
    return nullptr;
#endif // WINDOW_SCENE_SUPPORTED
}

void* createSaveButtonNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SaveButtonModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createScreenNode(ArkUI_Int32 nodeId)
{
#ifdef WINDOW_SCENE_SUPPORTED
    // auto frameNode = ScreenModel::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
#else
    return nullptr;
#endif // WINDOW_SCENE_SUPPORTED
}

void* createScrollBarNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ScrollBarModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createSecurityComponentMethodNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createShapeNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = ShapeModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createBaseSpanNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SpanModelNG::CreateSpanNode(nodeId, "");
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createStepperNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = StepperModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createStepperItemNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = StepperItemModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createSymbolGlyphNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SymbolModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createSymbolSpanNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SymbolSpanModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createVideoNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = VideoModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createWebNode(ArkUI_Int32 nodeId)
{
#ifdef WEB_SUPPORTED
    // auto frameNode = WebModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
#else
    return nullptr;
#endif // WEB_SUPPORTED
}

void* createWindowSceneNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createSideBarContainerNode(ArkUI_Int32 nodeId)
{
    // auto frameNode = SideBarContainerModelNG::CreateFrameNode(nodeId);
    // CHECK_NULL_RETURN(frameNode, nullptr);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
}

void* createRemoteWindowNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createUIExtensionComponentNode(ArkUI_Int32 nodeId)
{
#ifdef WINDOW_SCENE_SUPPORTED
    // auto frameNode = UIExtensionModelNG::CreateFrameNode(nodeId);
    // frameNode->IncRefCount();
    // return AceType::RawPtr(frameNode);
    return nullptr;
#else
    return nullptr;
#endif //WINDOW_SCENE_SUPPORTED
}

void* createLinearIndicatorNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createCustomNodeNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createIndicatorComponentNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}

void* createCommonTransitionNode(ArkUI_Int32 nodeId)
{
    return nullptr;
}
} // namespace OHOS::Ace::NG::ViewModel
