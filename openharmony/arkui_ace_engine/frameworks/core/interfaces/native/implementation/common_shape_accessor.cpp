/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "core/components_ng/base/frame_node.h"
#include "core/interfaces/native/utility/converter.h"
#include "arkoala_api_generated.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace CommonShapeAccessor {
void DestroyPeerImpl(Ark_CommonShape peer)
{
}
Ark_CommonShape CtorImpl()
{
    return {};
}
Ark_NativePointer GetFinalizerImpl()
{
    return reinterpret_cast<void *>(&DestroyPeerImpl);
}
Ark_CommonShape OffsetImpl(Ark_CommonShape peer,
                           const Ark_Position* offset)
{
    return {};
}
Ark_CommonShape FillImpl(Ark_CommonShape peer,
                         const Ark_ResourceColor* color)
{
    return {};
}
Ark_CommonShape PositionImpl(Ark_CommonShape peer,
                             const Ark_Position* position)
{
    return {};
}
} // CommonShapeAccessor
const GENERATED_ArkUICommonShapeAccessor* GetCommonShapeAccessor()
{
    static const GENERATED_ArkUICommonShapeAccessor CommonShapeAccessorImpl {
        CommonShapeAccessor::DestroyPeerImpl,
        CommonShapeAccessor::CtorImpl,
        CommonShapeAccessor::GetFinalizerImpl,
        CommonShapeAccessor::OffsetImpl,
        CommonShapeAccessor::FillImpl,
        CommonShapeAccessor::PositionImpl,
    };
    return &CommonShapeAccessorImpl;
}

struct CommonShapePeer {
    virtual ~CommonShapePeer() = default;
};
}
