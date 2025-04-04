/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/components/common/properties/border_edge.h"

#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

BorderEdge::BorderEdge(const Color& color, const Dimension& width, BorderStyle style)
    : color_(color), width_(width), style_(style)
{
    if (width.Value() < 0.0) {
        width_.SetValue(0.0);
    }
}

bool BorderEdge::HasValue() const
{
    return width_.Value() > 0.0;
}

void BorderEdge::SetContextAndCallback(
    const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
{
    width_.SetContextAndCallback(context, callback);
    color_.SetContextAndCallback(context, callback);
}

} // namespace OHOS::Ace
