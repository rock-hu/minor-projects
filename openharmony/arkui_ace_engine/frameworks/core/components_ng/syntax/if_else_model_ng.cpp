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

#include "core/components_ng/syntax/if_else_model_ng.h"

#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/syntax/if_else_node.h"

namespace OHOS::Ace::NG {

void IfElseModelNG::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto ifElseNode = IfElseNode::GetOrCreateIfElseNode(nodeId);
    stack->Push(ifElseNode);
}

void IfElseModelNG::Pop()
{
    ViewStackProcessor::GetInstance()->PopContainer();
}

void IfElseModelNG::SetBranchId(int32_t value, std::list<int32_t>& removedElmtId, std::list<int32_t>& reservedElmtId)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto node = AceType::DynamicCast<IfElseNode>(stack->GetMainElementNode());
    CHECK_NULL_VOID(node);
    node->SetBranchId(value, removedElmtId, reservedElmtId);
}

int32_t IfElseModelNG::GetBranchId()
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto node = AceType::DynamicCast<IfElseNode>(stack->GetMainElementNode());
    CHECK_NULL_RETURN(node, -1);
    return node->GetBranchId();
}

bool IfElseModelNG::CanRetake(const std::string& id)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto node = AceType::DynamicCast<IfElseNode>(stack->GetMainElementNode());
    CHECK_NULL_RETURN(node, false);
    return node->TryRetake(id);
}

bool IfElseModelNG::GetRetakenElmtIds(std::list<int32_t>& retakenElmtIds)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto node = AceType::DynamicCast<IfElseNode>(stack->GetMainElementNode());
    CHECK_NULL_RETURN(node, false);
    return node->GetRetakenElmtIds(retakenElmtIds);
}

} // namespace OHOS::Ace::NG
