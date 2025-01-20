/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "ecmascript/compiler/rt_call_signature.h"

#include "ecmascript/compiler/assembler_module.h"

namespace panda::ecmascript::kungfu {
CallSignature RuntimeStubCSigns::callSigns_[RuntimeStubCSigns::NUM_OF_RTSTUBS_WITHOUT_GC];
CallSignature RuntimeStubCSigns::optimizedCallSign_;
CallSignature RuntimeStubCSigns::optimizedFastCallSign_;

void RuntimeStubCSigns::Initialize()
{
#define INIT_SIGNATURES(name)                                        \
    name##CallSignature::Initialize(&callSigns_[ID_##name]);         \
    callSigns_[ID_##name].SetName(std::string("RTStub_") + #name);   \
    callSigns_[ID_##name].SetID(ID_##name);                          \
    assert(callSigns_[ID_##name].IsRuntimeNGCStub() ||               \
           callSigns_[ID_##name].IsRuntimeStub() ||                  \
           callSigns_[ID_##name].IsDeoptStub()   ||                  \
           callSigns_[ID_##name].IsRuntimeVAStub() ||                \
           callSigns_[ID_##name].IsASMCallBarrierStub());

    RUNTIME_STUB_WITHOUT_GC_LIST(INIT_SIGNATURES)
    RUNTIME_ASM_STUB_LIST(INIT_SIGNATURES)
#undef INIT_SIGNATURES

#define INIT_ASM_SIGNATURES(name)                                                     \
    callSigns_[RuntimeStubCSigns::ID_##name].SetName(std::string("RTStub_") + #name); \
    callSigns_[RuntimeStubCSigns::ID_##name].SetConstructor(                          \
        []([[maybe_unused]] void* arg) {                                              \
            return static_cast<void*>(new name##Stub());                              \
    });

    RUNTIME_ASM_STUB_LIST(INIT_ASM_SIGNATURES)
#undef INIT_ASM_SIGNATURES
    JSOptimizedCallCallSignature::Initialize(&optimizedCallSign_);
    JSOptimizedFastCallCallSignature::Initialize(&optimizedFastCallSign_);
}

void RuntimeStubCSigns::GetASMCSigns(std::vector<const CallSignature*>& outputCallSigns)
{
#define INIT_ASM_SIGNATURES(name) \
    outputCallSigns.push_back(&callSigns_[RuntimeStubCSigns::ID_##name]);

    RUNTIME_ASM_STUB_LIST(INIT_ASM_SIGNATURES)
#undef INIT_ASM_SIGNATURES
}
} // namespace panda::ecmascript::kungfu
