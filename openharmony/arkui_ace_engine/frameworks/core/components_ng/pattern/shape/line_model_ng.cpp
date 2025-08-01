/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SHAPE_LINE_MODEL_NG_CPP
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SHAPE_LINE_MODEL_NG_CPP

#include "core/components_ng/pattern/shape/line_model_ng.h"

#include "core/common/resource/resource_parse_utils.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/shape/line_paint_property.h"
#include "core/components_ng/pattern/shape/line_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t POINT_SIZE = 2;
}
void LineModelNG::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::LINE_ETS_TAG, nodeId);
    auto frameNode =
        FrameNode::GetOrCreateFrameNode(V2::LINE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<LinePattern>(); });
    stack->Push(frameNode);
}

void LineModelNG::StartPoint(const ShapePoint& value)
{
    ACE_UPDATE_PAINT_PROPERTY(LinePaintProperty, StartPoint, value);
}

void LineModelNG::EndPoint(const ShapePoint& value)
{
    ACE_UPDATE_PAINT_PROPERTY(LinePaintProperty, EndPoint, value);
}

void LineModelNG::StartPoint(FrameNode* frameNode, const ShapePoint& value)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(LinePaintProperty, StartPoint, value, frameNode);
}

void LineModelNG::EndPoint(FrameNode* frameNode, const ShapePoint& value)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(LinePaintProperty, EndPoint, value, frameNode);
}

void LineModelNG::StartPoint(const ShapePoint& value, const std::vector<RefPtr<ResourceObject>>& resObjArray)
{
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    StartPoint(frameNode, value, resObjArray);
}

void LineModelNG::StartPoint(
    FrameNode* frameNode, const ShapePoint& value, const std::vector<RefPtr<ResourceObject>>& resObjArray)
{
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    auto pattern = frameNode->GetPattern<ShapePattern>();
    CHECK_NULL_VOID(pattern);
    auto&& updateFunc = [frameNode, value, resObjArray](const RefPtr<ResourceObject>& resObj) {
        if (resObjArray.size() != POINT_SIZE) {
            return;
        }
        ShapePoint result = value;
        if (resObjArray[0]) {
            Dimension dim;
            ResourceParseUtils::ConvertFromResObjNG(resObjArray[0], dim);
            result.first = dim;
        }
        if (resObjArray[1]) {
            Dimension dim;
            ResourceParseUtils::ConvertFromResObjNG(resObjArray[1], dim);
            result.second = dim;
        }
        ACE_UPDATE_NODE_PAINT_PROPERTY(LinePaintProperty, StartPoint, result, frameNode);
    };
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>();
    pattern->AddResObj("LineStartPoint", resObj, std::move(updateFunc));
}

void LineModelNG::EndPoint(const ShapePoint& value, const std::vector<RefPtr<ResourceObject>>& resObjArray)
{
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    EndPoint(frameNode, value, resObjArray);
}

void LineModelNG::EndPoint(
    FrameNode* frameNode, const ShapePoint& value, const std::vector<RefPtr<ResourceObject>>& resObjArray)
{
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    auto pattern = frameNode->GetPattern<ShapePattern>();
    CHECK_NULL_VOID(pattern);
    auto&& updateFunc = [frameNode, value, resObjArray](const RefPtr<ResourceObject>& resObj) {
        if (resObjArray.size() != POINT_SIZE) {
            return;
        }
        ShapePoint result = value;
        if (resObjArray[0]) {
            Dimension dim;
            ResourceParseUtils::ConvertFromResObjNG(resObjArray[0], dim);
            result.first = dim;
        }
        if (resObjArray[1]) {
            Dimension dim;
            ResourceParseUtils::ConvertFromResObjNG(resObjArray[1], dim);
            result.second = dim;
        }
        ACE_UPDATE_NODE_PAINT_PROPERTY(LinePaintProperty, EndPoint, result, frameNode);
    };
    RefPtr<ResourceObject> resObj = AceType::MakeRefPtr<ResourceObject>();
    pattern->AddResObj("LineEndPoint", resObj, std::move(updateFunc));
}
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SHAPE_LINE_MODEL_NG_CPP