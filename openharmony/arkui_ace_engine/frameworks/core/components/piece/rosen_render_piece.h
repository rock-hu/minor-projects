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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PIECE_ROSEN_RENDER_PIECE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PIECE_ROSEN_RENDER_PIECE_H

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkRRect.h"
#endif

#include "core/components/piece/render_piece.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

class RosenRenderPiece : public RenderPiece {
    DECLARE_ACE_TYPE(RosenRenderPiece, RenderPiece);

public:
    void Paint(RenderContext& context, const Offset& offset) override;

private:
#ifndef USE_ROSEN_DRAWING
    SkRRect MakeRRect(const Offset& offset, const Size& size, const Border& border) const;
#else
    RSRoundRect MakeRRect(const Offset& offset, const Size& size, const Border& border) const;
#endif
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PIECE_ROSEN_RENDER_PIECE_H
