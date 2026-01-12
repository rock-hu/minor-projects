#include "napi/native_api.h"

  // [Start can_not_get_prototype_on_non_ecma_object_napi_case]
napi_value objvalues;
static napi_value Add(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = {nullptr};

    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    napi_valuetype valuetype0;
    napi_typeof(env, args[0], &valuetype0);

    napi_valuetype valuetype1;
    napi_typeof(env, args[1], &valuetype1);

    double value0;
    napi_get_value_double(env, args[0], &value0);

    double value1;
    napi_get_value_double(env, args[1], &value1);

    napi_value sum;
    napi_create_double(env, value0 + value1, &sum);
    
    napi_status status;
    napi_handle_scope scope;
    status = napi_open_handle_scope(env, &scope);
    napi_value jsArray = nullptr;
    napi_create_array(env, &jsArray);
    // 将创建好的数组进行赋值
    for (int i = 0; i < 5; i++) {
        napi_value element;
        napi_create_int32(env, i, &element);
        napi_set_element(env, jsArray, i, element);
    }
    status = napi_close_handle_scope(env, scope); // 作用域关闭，jsArray 被清理
    objvalues = jsArray; // 问题所在！
    
    return sum;

}

static napi_value GetValue(napi_env env, napi_callback_info info)
{
    return objvalues;
}
  // [End can_not_get_prototype_on_non_ecma_object_napi_case]

  // [Start out_of_memory_error_napi_case]
static napi_value TestLeak(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);
    napi_value proxy = args[0];
    napi_ref task_ref;
    napi_create_reference(env, proxy, 1, &task_ref);
    // 由于未调用napi_delete_reference发生内存泄漏
    // napi_delete_reference(env, task_ref->ref);
    return nullptr;
}
  // [End out_of_memory_error_napi_case]

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        { "add", nullptr, Add, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "TestLeak", nullptr, TestLeak, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}
