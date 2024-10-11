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
#ifndef OHOS_ACE_FRAMEWORK_CJ_FONT_MANAGER
#define OHOS_ACE_FRAMEWORK_CJ_FONT_MANAGER

#include "bridge/cj_frontend/frontend/cj_frontend_abstract.h"
#include "bridge/cj_frontend/interfaces/cj_ffi/cj_collection_ffi.h"
#include "bridge/cj_frontend/interfaces/cj_ffi/cj_common_ffi.h"
#include "bridge/cj_frontend/interfaces/cj_ffi/cj_macro.h"

extern "C" {
CJ_EXPORT void FfiFontManagerRegisterFont(const char* familyName, const char* familySrc);
CJ_EXPORT VectorStringHandle FfiFontManagerGetSystemFontList();
CJ_EXPORT NativeOptionFontInfo FfiFontManagerGetFontByName(const char* fontName);
}

#endif // OHOS_ACE_FRAMEWORK_CJ_FONT_MANAGER