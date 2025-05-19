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

#ifndef FRAMEWORKS_CORE_INTERFACES_NATIVE_NODE_NODE_IMAGE_MODIFIER_H
#define FRAMEWORKS_CORE_INTERFACES_NATIVE_NODE_NODE_IMAGE_MODIFIER_H

#include "core/interfaces/native/node/node_api.h"

namespace OHOS::Ace::NG::NodeModifier {
const ArkUIImageModifier* GetImageModifier();
const CJUIImageModifier* GetCJUIImageModifier();
void SetImageOnComplete(ArkUINodeHandle node, void* extraParam);
void SetImageOnError(ArkUINodeHandle node, void* extraParam);
void SetImageOnSvgPlayFinish(ArkUINodeHandle node, void* extraParam);
void SetImageOnDownloadProgress(ArkUINodeHandle node, void* extraParam);

void ResetImageOnComplete(ArkUINodeHandle node);
void ResetImageOnError(ArkUINodeHandle node);
void ResetImageOnSvgPlayFinish(ArkUINodeHandle node);
void ResetImageOnDownloadProgress(ArkUINodeHandle node);
} // namespace OHOS::Ace::NG::NodeModifier
#endif