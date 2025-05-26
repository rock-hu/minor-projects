//
// Created on 2025/5/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

/**
 * 最佳实践：CppCrash问题案例分析
 */

#include <js_native_api_types.h>

// [Start TriggerCrash1]
static napi_value TriggerCrash(napi_env env, napi_callback_info info)
{
    char *s = "hello world";
    s[1] = 'H';
    return 0;
}
// [End TriggerCrash1]
