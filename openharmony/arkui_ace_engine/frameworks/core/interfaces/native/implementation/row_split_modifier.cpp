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
#include "core/components_ng/pattern/linear_split/linear_split_model_ng.h"
#include "core/interfaces/native/utility/converter.h"
#include "core/interfaces/native/utility/reverse_converter.h"
#include "core/interfaces/native/generated/interface/node_api.h"
#include "core/components_ng/pattern/linear_split/linear_split_model_ng_static.h"

namespace OHOS::Ace::NG::GeneratedModifier {
namespace RowSplitModifier {
Ark_NativePointer ConstructImpl(Ark_Int32 id,
                                Ark_Int32 flags)
{
    auto frameNode = LinearSplitModelNGStatic::CreateFrameNode(id, NG::SplitType::ROW_SPLIT);
    CHECK_NULL_RETURN(frameNode, nullptr);
    frameNode->IncRefCount();
    return AceType::RawPtr(frameNode);
}
} // RowSplitModifier
namespace RowSplitInterfaceModifier {
void SetRowSplitOptionsImpl(Ark_NativePointer node)
{
    // keep it empty because RowSplit doesn`t have any options
}
} // RowSplitInterfaceModifier
namespace RowSplitAttributeModifier {
void ResizeableImpl(Ark_NativePointer node,
                    const Opt_Boolean* value)
{
    auto frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto convValue = Converter::OptConvert<bool>(*value);
    if (!convValue) {
        // TODO: Reset value
        return;
    }
    LinearSplitModelNG::SetResizable(frameNode, NG::SplitType::ROW_SPLIT, *convValue);
}
} // RowSplitAttributeModifier
const GENERATED_ArkUIRowSplitModifier* GetRowSplitModifier()
{
    static const GENERATED_ArkUIRowSplitModifier ArkUIRowSplitModifierImpl {
        RowSplitModifier::ConstructImpl,
        RowSplitInterfaceModifier::SetRowSplitOptionsImpl,
        RowSplitAttributeModifier::ResizeableImpl,
    };
    return &ArkUIRowSplitModifierImpl;
}

}
