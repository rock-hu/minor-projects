/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/particle/particle_model_ng.h"

#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/particle/particle_pattern.h"
namespace OHOS::Ace::NG {
void ParticleModelNG::Create(std::list<ParticleOption>& arrayValue)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::PARTICLE_ETS_TAG, nodeId);
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::PARTICLE_ETS_TAG, nodeId,
        [count = arrayValue.size()]() { return AceType::MakeRefPtr<ParticlePattern>(count); });
    stack->Push(frameNode);
    ACE_UPDATE_RENDER_CONTEXT(ParticleOptionArray, arrayValue);
}

void ParticleModelNG::DisturbanceField(const std::vector<ParticleDisturbance>& disturbanceArray, FrameNode* frameNode)
{
    auto pattern = AceType::DynamicCast<ParticlePattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(pattern);
    pattern->UpdateDisturbance(disturbanceArray);
}

void ParticleModelNG::DisturbanceField(const std::vector<ParticleDisturbance>& disturbanceArray)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto pattern = AceType::DynamicCast<ParticlePattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(pattern);
    pattern->UpdateDisturbance(disturbanceArray);
}

void ParticleModelNG::updateEmitter(std::vector<EmitterProperty>& property, FrameNode* frameNode)
{
    auto pattern = AceType::DynamicCast<ParticlePattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(pattern);
    pattern->updateEmitterPosition(property);
}

void ParticleModelNG::updateEmitter(std::vector<EmitterProperty>& property)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto pattern = AceType::DynamicCast<ParticlePattern>(frameNode->GetPattern());
    CHECK_NULL_VOID(pattern);
    pattern->updateEmitterPosition(property);
}
} // namespace OHOS::Ace::NG