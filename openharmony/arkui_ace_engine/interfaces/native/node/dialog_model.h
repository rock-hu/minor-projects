/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARKUI_NATIVE_NODE_DIALOG_MODEL_H
#define ARKUI_NATIVE_NODE_DIALOG_MODEL_H

#include "native_dialog.h"
#include "native_type.h"
#include "frameworks/core/interfaces/arkoala/arkoala_api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ArkUI_NativeDialog {
    ArkUIDialogHandle controller = nullptr;
};

struct ArkUI_DialogDismissEvent {
    void* userData;
    int32_t reason = -1;
    bool BlockDismiss = false;
};

#ifdef __cplusplus
};
#endif

namespace OHOS::Ace::DialogModel {
ArkUI_NativeDialogHandle Create();
void Dispose(ArkUI_NativeDialogHandle handle);
int32_t SetContent(ArkUI_NativeDialogHandle handle, ArkUI_NodeHandle content);
int32_t RemoveContent(ArkUI_NativeDialogHandle handle);
int32_t SetContentAlignment(ArkUI_NativeDialogHandle handle, int32_t alignment, float offsetX, float offsetY);
int32_t ResetContentAlignment(ArkUI_NativeDialogHandle handle);
int32_t SetModalMode(ArkUI_NativeDialogHandle handle, bool isModal);
int32_t SetAutoCancel(ArkUI_NativeDialogHandle handle, bool autoCancel);
int32_t SetMask(ArkUI_NativeDialogHandle handle, uint32_t maskColor, const ArkUI_Rect* rect);
int32_t SetBackgroundColor(ArkUI_NativeDialogHandle handle, uint32_t backgroundColor);
int32_t SetCornerRadius(ArkUI_NativeDialogHandle handle, float topLeft, float topRight,
    float bottomLeft, float bottomRight);
int32_t SetGridColumnCount(ArkUI_NativeDialogHandle handle, int32_t gridCount);
int32_t EnableCustomStyle(ArkUI_NativeDialogHandle handle, bool enableCustomStyle);
int32_t EnableCustomAnimation(ArkUI_NativeDialogHandle handle, bool enableCustomAnimation);
int32_t RegisterOnWillDismiss(ArkUI_NativeDialogHandle handle, ArkUI_OnWillDismissEvent eventHandler);
int32_t Show(ArkUI_NativeDialogHandle handle, bool showInSubWindow);
int32_t Close(ArkUI_NativeDialogHandle handle);
int32_t RegisterOnWillDismissWithUserData(
    ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(ArkUI_DialogDismissEvent* event));
int32_t SetKeyboardAvoidDistance(ArkUI_NativeDialogHandle handle, float distance, ArkUI_LengthMetricUnit unit);
int32_t SetLevelMode(ArkUI_NativeDialogHandle handle, ArkUI_LevelMode levelMode);
int32_t SetLevelUniqueId(ArkUI_NativeDialogHandle handle, int32_t uniqueId);
int32_t SetImmersiveMode(ArkUI_NativeDialogHandle handle, ArkUI_ImmersiveMode immersiveMode);
int32_t SetLevelOrder(ArkUI_NativeDialogHandle handle, double levelOrder);
int32_t RegisterOnWillAppear(ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(void* userData));
int32_t RegisterOnDidAppear(ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(void* userData));
int32_t RegisterOnWillDisappear(ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(void* userData));
int32_t RegisterOnDidDisappear(ArkUI_NativeDialogHandle handle, void* userData, void (*callback)(void* userData));
int32_t SetBorderWidth(
    ArkUI_NativeDialogHandle handle, float top, float right, float bottom, float left, ArkUI_LengthMetricUnit unit);
int32_t SetBorderColor(ArkUI_NativeDialogHandle handle, uint32_t top, uint32_t right, uint32_t bottom, uint32_t left);
int32_t SetBorderStyle(ArkUI_NativeDialogHandle handle, int32_t top, int32_t right, int32_t bottom, int32_t left);
int32_t SetWidth(ArkUI_NativeDialogHandle handle, float width, ArkUI_LengthMetricUnit unit);
int32_t SetHeight(ArkUI_NativeDialogHandle handle, float height, ArkUI_LengthMetricUnit unit);
int32_t SetShadow(ArkUI_NativeDialogHandle handle, ArkUI_ShadowStyle shadow);
int32_t SetCustomShadow(ArkUI_NativeDialogHandle handle, const ArkUI_AttributeItem* customShadow);
int32_t SetBackgroundBlurStyle(ArkUI_NativeDialogHandle handle, ArkUI_BlurStyle blurStyle);
int32_t SetKeyboardAvoidMode(ArkUI_NativeDialogHandle handle, ArkUI_KeyboardAvoidMode keyboardAvoidMode);
int32_t EnableHoverMode(ArkUI_NativeDialogHandle handle, bool enableHoverMode);
int32_t SetHoverModeArea(ArkUI_NativeDialogHandle handle, ArkUI_HoverModeAreaType hoverModeAreaType);
int32_t SetFocusable(ArkUI_NativeDialogHandle handle, bool focusable);
int32_t SetBackgroundBlurStyleOptions(ArkUI_NativeDialogHandle handle, const ArkUI_AttributeItem* customShadow);
int32_t SetBackgroundEffect(ArkUI_NativeDialogHandle handle, const ArkUI_AttributeItem* customShadow);
} // namespace OHOS::Ace::NG::DialogModel

#endif // ARKUI_NATIVE_NODE_DIALOG_MODEL_H