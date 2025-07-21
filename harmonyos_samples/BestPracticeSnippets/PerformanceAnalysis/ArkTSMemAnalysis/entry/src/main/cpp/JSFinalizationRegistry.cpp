/**
 * 源码请参考：https://gitee.com/openharmony/arkcompiler_ets_runtime
 */
#include "ecmascript/js_finalization_registry.h"

#include "ecmascript/global_env.h"
#include "ecmascript/jobs/micro_job_queue.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/linked_hash_table.h"

// [Start tagged_array_case1]
void JSFinalizationRegistry::CheckAndCall(JSThread *thread)
{
    if (thread->GetCheckAndCallEnterState()) {
        return;
    }
    CheckAndCallScope scope(thread);
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> prev = env->GetFinRegLists();

    if (prev->IsUndefined()) {
        return;
    }
    JSMutableHandle<JSTaggedValue> current(thread, prev.GetTaggedValue());
    JSMutableHandle<JSFinalizationRegistry> jsFinalizationRegistry(thread, current.GetTaggedValue());
    while (!current->IsNull()) {
        jsFinalizationRegistry.Update(current.GetTaggedValue());
        current.Update(jsFinalizationRegistry->GetPrev());
        if (CleanupFinalizationRegistry(thread, jsFinalizationRegistry)) { // -> 嵌套调用处
            // If the objects registered on the current JSFinalizationRegistry object have all been gc,
            // then we clean up this JSFinalizationRegistry object from the FinRegLists
            CleanFinRegLists(thread, jsFinalizationRegistry);
        }
    }
}

bool JSFinalizationRegistry::CleanupFinalizationRegistry(JSThread *thread, JSHandle<JSFinalizationRegistry> obj)
{
    ASSERT(obj->IsECMAObject());
    auto ecmaVm = thread->GetEcmaVM();
    JSHandle<job::MicroJobQueue> job = ecmaVm->GetMicroJobQueue();
    ObjectFactory *factory = ecmaVm->GetFactory();
    JSHandle<JSFunction> func(thread, obj->GetCleanupCallback());
    JSHandle<CellRecordVector> noUnregister(thread, obj->GetNoUnregister());
    if (!noUnregister->Empty()) {
        uint32_t noUnregisterLen = noUnregister->GetEnd();
        for (uint32_t i = 0; i < noUnregisterLen; ++i) {
            // ...
        }
    }
    JSMutableHandle<LinkedHashMap> maybeUnregister(thread, obj->GetMaybeUnregister()); // -> 存在多个tag array嵌套
    // ...
}
// [End tagged_array_case1]

// [Start tagged_array_case2]
void JSFinalizationRegistry::CheckAndCall(JSThread *thread)
{
    if (thread->GetCheckAndCallEnterState()) {
        return;
    }
    [[maybe_unused]] EcmaHandleScope handleScope(thread); // -> 添加handleScope
    CheckAndCallScope scope(thread);
    EcmaVM* vm = thread->GetEcmaVM();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> prev = env->GetFinRegLists();

    if (prev->IsUndefined()) {
        return;
    }
    JSMutableHandle<JSTaggedValue> current(thread, prev.GetTaggedValue());
    JSMutableHandle<JSFinalizationRegistry> jsFinalizationRegistry(thread, current.GetTaggedValue());
    while (!current->IsNull()) {
        jsFinalizationRegistry.Update(current.GetTaggedValue());
        current.Update(jsFinalizationRegistry->GetPrev());
        if (CleanupFinalizationRegistry(thread, jsFinalizationRegistry)) {
            // If the objects registered on the current JSFinalizationRegistry object have all been gc,
            // then we clean up this JSFinalizationRegistry object from the FinRegLists
            CleanFinRegLists(thread, jsFinalizationRegistry);
        }
    }
}
// [End tagged_array_case2]