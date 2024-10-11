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

#ifndef PANDA_RUNTIME_TESTS_INVOCATION_HELPER_H_
#define PANDA_RUNTIME_TESTS_INVOCATION_HELPER_H_

#include <cstdint>
#include <type_traits>

#include "bridge/bridge.h"
#include "include/managed_thread.h"
#include "arch/helpers.h"
#include "libpandafile/shorty_iterator.h"

namespace ark::test {

const void *GetInvokeHelperImpl();

template <typename T>
auto GetInvokeHelper()
{
    using Fn = T (*)(const uint8_t *, const uint8_t *, const uint8_t *, size_t, ark::ManagedThread *);
    return reinterpret_cast<Fn>(const_cast<void *>(GetInvokeHelperImpl()));
}

inline void WriteArgImpl(arch::ArgWriter<RUNTIME_ARCH> * /* unused */, size_t /* unused */) {}

template <typename T, typename... Args>
inline void WriteArgImpl(arch::ArgWriter<RUNTIME_ARCH> *writer, size_t nfloats, T arg, Args... args);

template <typename... Args>
inline void WriteArgImpl(arch::ArgWriter<RUNTIME_ARCH> *writer, size_t nfloats, float arg, Args... args)
{
    writer->Write(arg);
    WriteArgImpl(writer, nfloats + 1, args...);
}

template <typename T, typename... Args>
inline void WriteArgImpl(arch::ArgWriter<RUNTIME_ARCH> *writer, size_t nfloats, T arg, Args... args)
{
    if (RUNTIME_ARCH == Arch::AARCH32 && std::is_same_v<double, T>) {
        // JIT compiler doesn't pack floats according armhf ABI. So in the following case:
        //
        // void foo(f32 a0, f64 a1, f32 a2)
        //
        // Arguments will be passed in the following registers:
        // a0 - s0
        // a1 - d1
        // a2 - s4
        //
        // But according to armhf ABI a0 and a2 should be packed into d0:
        // a0 - s0
        // a1 - d1
        // a2 - s1
        //
        // So write additional float if necessary to prevent packing
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        if ((nfloats & 0x1) != 0) {
            nfloats += 1;
            writer->Write(0.0F);
        }
    }
    writer->Write(arg);
    WriteArgImpl(writer, nfloats, args...);
}

template <class T, typename... Args>
inline void WriteArg(arch::ArgWriter<RUNTIME_ARCH> *writer, T arg, Args... args)
{
    WriteArgImpl(writer, 0, arg, args...);
}

template <typename T>
inline T InvokeEntryPoint(Method *method)
{
    PandaVector<uint8_t> gprData(arch::ExtArchTraits<RUNTIME_ARCH>::GP_ARG_NUM_BYTES);
    Span<uint8_t> gprs(gprData.data(), gprData.size());
    PandaVector<uint8_t> fprData(arch::ExtArchTraits<RUNTIME_ARCH>::FP_ARG_NUM_BYTES);
    Span<uint8_t> fprs(fprData.data(), fprData.size());
    PandaVector<uint8_t> stack;
    arch::ArgWriter<RUNTIME_ARCH> writer(gprs, fprs, stack.data());
    writer.Write(method);

    ManagedThread *thread = ManagedThread::GetCurrent();
    return GetInvokeHelper<T>()(gprData.data(), fprData.data(), stack.data(), 0, thread);
}

void CountMethodTypes(panda_file::ShortyIterator &it, arch::ArgCounter<RUNTIME_ARCH> counter);

template <typename T, typename... Args>
T InvokeEntryPoint(Method *method, Args... args)
{
    arch::ArgCounter<RUNTIME_ARCH> counter;
    counter.Count<Method *>();
    if (!method->IsStatic()) {
        counter.Count<ObjectHeader *>();
    }
    panda_file::ShortyIterator it(method->GetShorty());
    ++it;  // skip return type
    while (it != panda_file::ShortyIterator()) {
        CountMethodTypes(it, counter);
        ++it;
    }

    PandaVector<uint8_t> gprData(arch::ExtArchTraits<RUNTIME_ARCH>::GP_ARG_NUM_BYTES);
    Span<uint8_t> gprs(gprData.data(), gprData.size());
    PandaVector<uint8_t> fprData(arch::ExtArchTraits<RUNTIME_ARCH>::FP_ARG_NUM_BYTES);
    Span<uint8_t> fprs(fprData.data(), fprData.size());
    PandaVector<uint8_t> stack(counter.GetStackSpaceSize());
    arch::ArgWriter<RUNTIME_ARCH> writer(gprs, fprs, stack.data());
    writer.Write(method);
    WriteArg(&writer, args...);

    ManagedThread *thread = ManagedThread::GetCurrent();
    return GetInvokeHelper<T>()(gprData.data(), fprData.data(), stack.data(), counter.GetStackSize(), thread);
}

template <typename... Args>
coretypes::TaggedValue InvokeDynEntryPoint(Method *method, uint32_t numArgs, Args... args)
{
    arch::ArgCounter<RUNTIME_ARCH> counter;
    counter.Count<Method *>();
    counter.Count<uint32_t>();
    for (uint32_t i = 0; i <= numArgs; ++i) {
        counter.Count<coretypes::TaggedValue>();
    }

    PandaVector<uint8_t> gprData(arch::ExtArchTraits<RUNTIME_ARCH>::GP_ARG_NUM_BYTES);
    Span<uint8_t> gprs(gprData.data(), gprData.size());
    PandaVector<uint8_t> fprData(arch::ExtArchTraits<RUNTIME_ARCH>::FP_ARG_NUM_BYTES);
    Span<uint8_t> fprs(fprData.data(), fprData.size());
    PandaVector<uint8_t> stack(counter.GetStackSpaceSize());
    arch::ArgWriter<RUNTIME_ARCH> writer(gprs, fprs, stack.data());
    writer.Write(method);
    writer.Write(numArgs);
    WriteArg(&writer, args...);

    ManagedThread *thread = ManagedThread::GetCurrent();
    return GetInvokeHelper<coretypes::TaggedValue>()(gprData.data(), fprData.data(), stack.data(),
                                                     counter.GetStackSize(), thread);
}

}  // namespace ark::test

#endif  // PANDA_RUNTIME_TESTS_INVOCATION_HELPER_H_
