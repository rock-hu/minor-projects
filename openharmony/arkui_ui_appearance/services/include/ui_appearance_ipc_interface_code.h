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

#ifndef UI_APPEARANCE_IPC_INTERFACE_CODE_H
#define UI_APPEARANCE_IPC_INTERFACE_CODE_H

/* SAID: 7002 */
namespace OHOS {
namespace ArkUi::UiAppearance {
enum class UiAppearanceInterfaceCode {
    SET_DARK_MODE = 1,
    GET_DARK_MODE = 2,
    GET_FONT_SCALE = 3,
    SET_FONT_SCALE = 4,
    GET_FONT_Weight_SCALE = 5,
    SET_FONT_Weight_SCALE = 6,
};
} // namespace ArkUi::UiAppearance
} // namespace OHOS

#endif // UI_APPEARANCE_IPC_INTERFACE_CODE_H