/*c
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARKUI_NATIVE_NODE_COMPONENT_SNAPSHOT_H
#define ARKUI_NATIVE_NODE_COMPONENT_SNAPSHOT_H

#include "native_type.h"

#ifdef __cplusplus
extern "C" {
#endif

ArkUI_SnapshotOptions* OH_ArkUI_CreateSnapshotOptions();

void OH_ArkUI_DestroySnapshotOptions(ArkUI_SnapshotOptions* snapshotOptions);

int32_t OH_ArkUI_SnapshotOptions_SetScale(ArkUI_SnapshotOptions* snapshotOptions, float scale);

int32_t OH_ArkUI_GetNodeSnapshot(
    ArkUI_NodeHandle node, ArkUI_SnapshotOptions* snapshotOptions, OH_PixelmapNative** pixelmap);

#ifdef __cplusplus
};
#endif
#endif // ARKUI_NATIVE_NODE_COMPONENT_SNAPSHOT_H
