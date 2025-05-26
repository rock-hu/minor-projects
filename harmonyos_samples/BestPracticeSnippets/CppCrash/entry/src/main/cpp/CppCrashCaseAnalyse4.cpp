//
// Created on 2025/5/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

/**
 * 最佳实践：CppCrash问题案例分析
 */

#include <cstdio>
#include <cstdlib>
#include <js_native_api_types.h>

// [Start TriggerCrash4]
static napi_value TriggerCrash(napi_env env, napi_callback_info info)
{
    void *pc = malloc(1024);
    free(pc);
    free(pc);  // repetitive free
    printf("free ok!\n");
    return 0;
}
// [End TriggerCrash4]
