
/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARKCOMPILER_TOOLCHAIN_INSPECTOR_LIBRARY_LOADER_H
#define ARKCOMPILER_TOOLCHAIN_INSPECTOR_LIBRARY_LOADER_H

#include <string_view>

namespace OHOS::ArkCompiler::Toolchain {
void* Load(std::string_view libraryName);
void* ResolveSymbol(void* handle, std::string_view symbol);
void CloseHandle(void* handle);
} // namespace OHOS::ArkCompiler::Toolchain

#endif // ARKCOMPILER_TOOLCHAIN_INSPECTOR_LIBRARY_LOADER_H
