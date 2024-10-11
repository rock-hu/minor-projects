/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "get_test_class.h"

#include "ets_vm.h"
#include "ets_class_linker_extension.h"
#include "assembly-emitter.h"
#include "assembly-parser.h"

namespace ark::ets::test {

EtsClass *GetTestClass(const char *source, const char *className, const std::string &sourceFilename)
{
    pandasm::Parser p;

    auto res = p.Parse(source, sourceFilename);
    auto pf = pandasm::AsmEmitter::Emit(res.Value());

    LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);

    auto classLinker = Runtime::GetCurrent()->GetClassLinker();
    ASSERT(classLinker);
    classLinker->AddPandaFile(std::move(pf));

    EtsCoroutine *coroutine = EtsCoroutine::GetCurrent();
    EtsClassLinker *etsClassLinker = coroutine->GetPandaVM()->GetClassLinker();

    EtsClass *klass = etsClassLinker->GetClass(className);
    if (klass == nullptr) {
        return nullptr;
    }

    ctx.InitializeClass(classLinker, coroutine, klass->GetRuntimeClass());

    return klass;
}

}  // namespace ark::ets::test
