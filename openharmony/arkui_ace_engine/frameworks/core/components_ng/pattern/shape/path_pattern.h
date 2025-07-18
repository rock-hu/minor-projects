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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SHAPE_PATH_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SHAPE_PATH_PATTERN_H

#include <string>
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/utils/noncopyable.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/shape/path_layout_algorithm.h"
#include "core/components_ng/pattern/shape/path_paint_method.h"
#include "core/components_ng/pattern/shape/path_paint_property.h"
#include "core/components_ng/pattern/shape/shape_layout_algorithm.h"
#include "core/components_ng/pattern/shape/shape_paint_property.h"
#include "core/components_ng/pattern/shape/shape_pattern.h"

namespace OHOS::Ace::NG {
class PathPattern : public ShapePattern {
    DECLARE_ACE_TYPE(PathPattern, ShapePattern);

public:
    PathPattern() = default;
    ~PathPattern() override = default;

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        if (!shapeOverlayModifier_) {
            shapeOverlayModifier_ = MakeRefPtr<ShapeOverlayModifier>();
        }
        return MakeRefPtr<PathPaintMethod>(GetAncestorPaintProperty(), shapeOverlayModifier_);
    }

    RefPtr<PaintProperty> CreatePaintProperty() override
    {
        return MakeRefPtr<PathPaintProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<PathLayoutAlgorithm>(GetAncestorPaintProperty());
    }

    void UpdatePropertyImpl(const std::string& key, RefPtr<PropertyValueBase> value) override
    {
        ShapePattern::UpdatePropertyImpl(key, value);
        auto frameNode = GetHost();
        CHECK_NULL_VOID(frameNode);
        if (key == "PathCommands") {
            if (auto realValue = std::get_if<std::string>(&(value->GetValue()))) {
                ACE_UPDATE_NODE_PAINT_PROPERTY(PathPaintProperty, Commands, *realValue, frameNode);
            }
        }
    }

private:
    ACE_DISALLOW_COPY_AND_MOVE(PathPattern);
    void SetClipBounds()
    {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto paintProperty = host->GetPaintProperty<PathPaintProperty>();
        CHECK_NULL_VOID(host);
        auto cmdStr = paintProperty->GetCommands().value_or("");
        if (cmdStr != lastCommands_) {
            lastCommands_ = cmdStr;
            auto renderContext = host->GetRenderContext();
            CHECK_NULL_VOID(renderContext);
            renderContext->SetClipBoundsWithCommands(cmdStr);
        }
    }

    std::string lastCommands_ = "";
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SHAPE_PATH_PATTERN_H
