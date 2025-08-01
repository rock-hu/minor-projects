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

#ifndef FOUNDATION_ACE_NAPI_INTERFACES_KITS_NAPI_NATIVE_NODE_API_H
#define FOUNDATION_ACE_NAPI_INTERFACES_KITS_NAPI_NATIVE_NODE_API_H

#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "js_native_api.h"
#include "node_api.h"
#include "native_common.h"

typedef void (*NAPIGetJSCode)(const char** buf, int* bufLen);
typedef void (*NapiNativeFinalize)(napi_env env, void* data, void* hint);
typedef void* (*NapiDetachCallback)(napi_env env, void* nativeObject, void* hint); // hint: detach params
typedef napi_value (*NapiAttachCallback)(napi_env env, void* nativeObject, void* hint); // hint: attach params
typedef bool (*napi_module_validate_callback)(const char* moduleName);
typedef struct napi_fast_native_scope__* napi_fast_native_scope;

typedef struct napi_module_with_js {
    int nm_version = 0;
    unsigned int nm_flags = 0;
    const char* nm_filename = nullptr;
    napi_addon_register_func nm_register_func = nullptr;
    const char* nm_modname = nullptr;
    void* nm_priv = nullptr;
    NAPIGetJSCode nm_get_abc_code = nullptr;
    NAPIGetJSCode nm_get_js_code = nullptr;
} napi_module_with_js;

typedef struct napi_stack_info {
    size_t stack_start;
    size_t stack_size;
} napi_stack_info;

typedef enum {
    napi_eprio_vip = 0,
    napi_eprio_immediate = 1,
    napi_eprio_high = 2,
    napi_eprio_low = 3,
    napi_eprio_idle = 4,
} napi_event_priority;

typedef enum {
    NAPI_APP_STATE_FOREGROUND = 0,
    NAPI_APP_STATE_BACKGROUND = 1,
    NAPI_APP_STATE_SENSITIVE_START = 2,
    NAPI_APP_STATE_SENSITIVE_END = 3,
    NAPI_APP_STATE_COLD_START_FINISHED = 4,
    NAPI_APP_STATE_WARM_START = 5,
} NapiAppState;

using NapiAppStateCallback = void (*)(int state, int64_t timestamp);

NAPI_EXTERN napi_status napi_create_limit_runtime(napi_env env, napi_env* result_env);
NAPI_EXTERN void napi_module_with_js_register(napi_module_with_js* mod);
NAPI_EXTERN napi_status napi_is_callable(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_create_runtime(napi_env env, napi_env* result_env);
NAPI_EXTERN napi_status napi_serialize_inner(napi_env env, napi_value object, napi_value transfer_list,
                                             napi_value clone_list, bool defaultTransfer, bool defaultCloneSendable,
                                             void** result);
NAPI_EXTERN napi_status napi_serialize_inner_with_error(napi_env env, napi_value object, napi_value transfer_list,
                                                        napi_value clone_list, bool defaultTransfer,
                                                        bool defaultCloneSendable, void** result, std::string& error);
NAPI_EXTERN napi_status napi_run_actor(napi_env env,
                                       const char* path,
                                       char* entryPoint,
                                       napi_value* result);
NAPI_EXTERN napi_status napi_wrap_with_size(napi_env env,
                                            napi_value js_object,
                                            void* native_object,
                                            napi_finalize finalize_cb,
                                            void* finalize_hint,
                                            napi_ref* result,
                                            size_t native_binding_size);
NAPI_EXTERN napi_status napi_wrap_async_finalizer(napi_env env,
                                                  napi_value js_object,
                                                  void* native_object,
                                                  napi_finalize finalize_cb,
                                                  void* finalize_hint,
                                                  napi_ref* result,
                                                  size_t native_binding_size);
NAPI_EXTERN napi_status napi_create_external_with_size(napi_env env,
                                                       void* data,
                                                       napi_finalize finalize_cb,
                                                       void* finalize_hint,
                                                       napi_value* result,
                                                       size_t native_binding_size);
NAPI_EXTERN napi_status napi_set_promise_rejection_callback(napi_env env, napi_ref ref, napi_ref checkRef);

NAPI_EXTERN napi_status napi_is_arguments_object(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_async_function(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_boolean_object(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_generator_function(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_date(napi_env env, napi_value value, bool* result);

NAPI_EXTERN napi_status napi_is_map_iterator(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_set_iterator(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_generator_object(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_module_namespace_object(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_proxy(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_reg_exp(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_number_object(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_map(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_set(napi_env env, napi_value value, bool* result);

NAPI_EXTERN napi_status napi_is_string_object(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_symbol_object(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_weak_map(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_weak_set(napi_env env, napi_value value, bool* result);

NAPI_EXTERN napi_status napi_is_big_int64_array(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_big_uint64_array(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_is_shared_array_buffer(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_get_stack_trace(napi_env env, std::string& stack);
NAPI_EXTERN napi_status napi_get_hybrid_stack_trace(napi_env env, std::string& stack);
NAPI_EXTERN napi_status napi_get_own_property_descriptor(napi_env env,
                                                         napi_value object,
                                                         const char* utf8name,
                                                         napi_value* result);
NAPI_EXTERN napi_status napi_object_get_keys(napi_env env, napi_value data, napi_value* result);
NAPI_EXTERN napi_status napi_get_print_string(napi_env env,
                                              napi_value value,
                                              std::string& result);
/*
 * @brief Send a task to the JS Thread
 *
 * @param env The native engine.
 * @param cb CallBack in JS Thread
 * @param priority Task priority
 *
 * @return napi_status Return send event status
 */
NAPI_EXTERN napi_status napi_send_event(napi_env env,
                                        const std::function<void()>& cb,
                                        napi_event_priority priority,
                                        const char* name = nullptr);
/*
 * @brief Send a task to the JS Thread
 *
 * @param env The native engine.
 * @param cb CallBack in JS Thread.
 * @param data Input parameter of callback.
 * @param priority Task priority.
 * @param handleId Handle to cancel a task.
 * @param name Task name, Use the task name and handleId as the tag of task.
 *
 * @return napi_status Return send event status
 */
NAPI_EXTERN napi_status napi_send_cancelable_event(napi_env env,
                                                   const std::function<void(void*)>& cb,
                                                   void* data,
                                                   napi_event_priority priority,
                                                   uint64_t* handleId,
                                                   const char* name);
/*
 * @brief Send a task to the JS Thread
 *
 * @param env The native engine.
 * @param handleId Handle to cancel a task, But sometimes it do not work when used alone.
 * @param name Task name, The name must be the same as the name of the task send.
 *
 * @return napi_status Return cancel event status
 */
NAPI_EXTERN napi_status napi_cancel_event(napi_env env, uint64_t handleId, const char* name);
NAPI_EXTERN napi_status napi_open_fast_native_scope(napi_env env, napi_fast_native_scope* scope);
NAPI_EXTERN napi_status napi_close_fast_native_scope(napi_env env, napi_fast_native_scope scope);
NAPI_EXTERN napi_status napi_get_shared_array_buffer_info(napi_env env,
                                                          napi_value arraybuffer,
                                                          void** data,
                                                          size_t* byte_length);
NAPI_EXTERN napi_status napi_encode(napi_env env, napi_value src, napi_value* result);
NAPI_EXTERN napi_status napi_is_bitvector(napi_env env, napi_value value, bool* result);
NAPI_EXTERN napi_status napi_add_cleanup_finalizer(napi_env env, void (*fun)(void* arg), void* arg);
NAPI_EXTERN napi_status napi_remove_cleanup_finalizer(napi_env env, void (*fun)(void* arg), void* arg);
/**
 * Sets a callback to validate modules when they are loaded by the context engine.
 *
 * This function is thread-safe.
 *
 * @param check_callback The callback used to determine whether a module can be imported.
 *                       The callback should return true if the module name is allowed.
 *
 * @return napi_status The status of the operation. Returns napi_ok if successful.
 */
NAPI_EXTERN napi_status napi_set_module_validate_callback(napi_module_validate_callback check_callback);
NAPI_EXTERN napi_status napi_set_stackinfo(napi_env env, napi_stack_info *napi_info);
NAPI_EXTERN napi_status napi_get_stackinfo(napi_env env, napi_stack_info *result);
NAPI_EXTERN napi_status napi_load_module_with_path(napi_env env, const char* path,
                                                   napi_value* result);
NAPI_EXTERN napi_status napi_load_module_with_module_request(napi_env env, const char* request_name,
                                                             napi_value* result);
NAPI_EXTERN napi_status napi_throw_jsvalue(napi_env env, napi_value error);
typedef enum {
    NAPI_DIRECTION_INVALID = 0,
    NAPI_DIRECTION_DYNAMIC_TO_STATIC = 1, // JS object references the STS object
    NAPI_DIRECTION_STATIC_TO_DYNAMIC = 2, // STS object references the JS object
    NAPI_DIRECTION_HYBRID = 3, // STS object and the JS object references each other
} NapiXRefDirection;

#ifdef PANDA_JS_ETS_HYBRID_MODE
// XGC specific internal API
NAPI_EXTERN napi_status napi_vm_handshake(napi_env env, void* inputIface, void** outputIface);
NAPI_EXTERN napi_status napi_xref_wrap(napi_env env,
                                       napi_value js_object,
                                       void* native_object,
                                       napi_finalize finalize_cb,
                                       NapiXRefDirection ref_direction,
                                       napi_ref* result);
NAPI_EXTERN napi_status napi_xref_unwrap(napi_env env, napi_value js_object, void** result);

NAPI_EXTERN napi_status napi_mark_from_object(napi_env env, napi_ref ref);
NAPI_EXTERN napi_status napi_create_xref(napi_env env,
                                         napi_value value,
                                         uint32_t initial_refcount,
                                         napi_ref* result);
NAPI_EXTERN napi_status napi_wrap_with_xref(napi_env env,
                                            napi_value js_object,
                                            void* native_object,
                                            napi_finalize finalize_cb,
                                            napi_ref* result);
NAPI_EXTERN napi_status napi_is_alive_object(napi_env env, napi_ref ref, bool* result);
NAPI_EXTERN napi_status napi_is_contain_object(napi_env env, napi_ref ref, bool* result);
NAPI_EXTERN napi_status napi_is_xref_type(napi_env env, napi_value js_object, bool* result);
NAPI_EXTERN napi_status napi_get_ets_implements(napi_env env, napi_value value, napi_value* result);
NAPI_EXTERN napi_status napi_setup_hybrid_environment(napi_env env);
#endif  // PANDA_JS_ETS_HYBRID_MODE
NAPI_EXTERN napi_status napi_is_alive_object(napi_env env, napi_ref ref, bool* result);
NAPI_EXTERN napi_status napi_is_contain_object(napi_env env, napi_ref ref, bool* result);
NAPI_EXTERN napi_status napi_is_xref_type(napi_env env, napi_value js_object, bool* result);
NAPI_EXTERN napi_status napi_register_appstate_callback(napi_env env, NapiAppStateCallback callback);
NAPI_EXTERN napi_status napi_load_module_with_info_hybrid(napi_env env,
                                                          const char* path,
                                                          const char* module_info,
                                                          napi_value* result);

#endif /* FOUNDATION_ACE_NAPI_INTERFACES_KITS_NAPI_NATIVE_NODE_API_H */
