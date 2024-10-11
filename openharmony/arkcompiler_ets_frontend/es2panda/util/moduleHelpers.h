/*
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

#ifndef ES2PANDA_UTIL_MODULE_HELPERS_H
#define ES2PANDA_UTIL_MODULE_HELPERS_H

#include <assembly-program.h>
#include <es2panda.h>
#include <mem/arena_allocator.h>
#include <util/programCache.h>

namespace panda::es2panda::util {
class ModuleHelpers {
public:
    static void CompileNpmModuleEntryList(const std::string &entriesInfo, panda::es2panda::CompilerOptions &options,
        std::map<std::string, panda::es2panda::util::ProgramCache*> &progsInfo, panda::ArenaAllocator *allocator);
};
} // panda::es2panda::util

#endif
