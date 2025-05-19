/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_NAPI_NATIVE_ENGINE_NATIVE_ASYNC_WORK_H
#define FOUNDATION_ACE_NAPI_NATIVE_ENGINE_NATIVE_ASYNC_WORK_H

#include "interfaces/kits/napi/common.h"
#include "native_value.h"
#ifdef ENABLE_HITRACE
#include "hitrace/trace.h"
#endif
#include <mutex>
#include <queue>
#include <uv.h>

struct NativeAsyncWorkDataPointer {
    NativeAsyncWorkDataPointer()
    {
        data_ = nullptr;
    }

    explicit NativeAsyncWorkDataPointer(void* data)
    {
        data_ = data;
    }
    void* data_ { nullptr };
};

#ifdef ENABLE_HITRACE
namespace OHOS {
namespace HiviewDFX {
class HiTraceId;
}
} // namespace OHOS
#endif

class NativeAsyncWork {
public:
    NativeAsyncWork(NativeEngine* engine,
                    NativeAsyncExecuteCallback execute,
                    NativeAsyncCompleteCallback complete,
                    const std::string &asyncResourceName,
                    void* data);

    virtual ~NativeAsyncWork();
    virtual bool Queue(NativeEngine* engine);
    virtual bool QueueWithQos(NativeEngine* engine, napi_qos_t qos);
    virtual bool Cancel(NativeEngine* engine);
    virtual std::string GetTraceDescription();
    template<typename Inner, typename Outer>
    static Outer* DereferenceOf(const Inner Outer::*field, const Inner* pointer)
    {
        if (field != nullptr && pointer != nullptr) {
            auto fieldOffset = reinterpret_cast<uintptr_t>(&(static_cast<Outer*>(0)->*field));
            auto outPointer = reinterpret_cast<Outer*>(reinterpret_cast<uintptr_t>(pointer) - fieldOffset);
            return outPointer;
        }
        return nullptr;
    }

private:
    static void AsyncWorkCallback(uv_work_t* req);
    static void AsyncAfterWorkCallback(uv_work_t* req, int status);

    uv_work_t work_;
    NativeEngine* engine_;
    uint64_t engineId_;
    NativeAsyncExecuteCallback execute_;
    NativeAsyncCompleteCallback complete_;
    void* data_;
    std::mutex workAsyncMutex_;
    std::queue<NativeAsyncWorkDataPointer> asyncWorkRecvData_;
    std::string traceDescription_;
#ifdef ENABLE_CONTAINER_SCOPE
    int32_t containerScopeId_;
#endif
#ifdef ENABLE_HITRACE
    OHOS::HiviewDFX::HiTraceId taskTraceId_;
#endif
};

#endif /* FOUNDATION_ACE_NAPI_NATIVE_ENGINE_NATIVE_ASYNC_WORK_H */
