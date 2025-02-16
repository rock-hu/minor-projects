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

#ifndef ES2PANDA_UTIL_PROGRAM_CACHE_H
#define ES2PANDA_UTIL_PROGRAM_CACHE_H

#include <assembly-program.h>
#include <assemblyProgramProto.h>

namespace panda::es2panda::util {
struct ProgramCache {
    uint32_t hashCode { 0 };
    panda::pandasm::Program program;
    bool needUpdateCache {false};

    explicit ProgramCache(panda::pandasm::Program program) : program(std::move(program))
    {
    }

    ProgramCache(uint32_t hashCode, panda::pandasm::Program program) : hashCode(hashCode), program(std::move(program))
    {
    }

    ProgramCache(uint32_t hashCode, panda::pandasm::Program program, bool needUpdateCache)
        : hashCode(hashCode), program(std::move(program)), needUpdateCache(needUpdateCache)
    {
    }
};

struct AbcProgramsCache {
    uint32_t hashCode { 0 };
    /**
     * The hashcode in ProgramCache corresponds to the hash value of the abc file, so the hashCode in ProgramCache is
     * the same as the hashcode in AbcProgramsCache
     */
    std::map<std::string, ProgramCache *> programsCache {};

    AbcProgramsCache(uint32_t hashCode, std::map<std::string, ProgramCache *> programsCache)
        : hashCode(hashCode), programsCache(std::move(programsCache))
    {
    }
};
} //panda::es2panda::util
#endif
