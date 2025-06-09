//
// Created on 2025/5/28.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
/**
 * libuv使用规范及案例
 */

#ifndef LIBUVDEVELOPMENT_LIBUVHANDLENORMAL_H
#define LIBUVDEVELOPMENT_LIBUVHANDLENORMAL_H
// [Start libuv_handle_normal]
/* Handle representing an event loop */
typedef struct uv_loop_s uv_loop_t;
/* The parent structure of all handles */
typedef struct uv_handle_s uv_handle_t;
/* Handle used to represent timer tasks */
typedef struct uv_timer_s uv_timer_t;
/* Handle used to represent communication between threads */
typedef struct uv_async_s uv_async_t;
// [End libuv_handle_normal]
#endif //LIBUVDEVELOPMENT_LIBUVHANDLENORMAL_H
