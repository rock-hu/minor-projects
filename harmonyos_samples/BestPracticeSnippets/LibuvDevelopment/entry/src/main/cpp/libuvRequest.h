//
// Created on 2025/5/28.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
/**
 * libuv使用规范及案例
 */
#ifndef LIBUVDEVELOPMENT_LIBUVREQUEST_H
#define LIBUVDEVELOPMENT_LIBUVREQUEST_H
// [Start libuv_request]
typedef struct uv_req_s uv_req_t;
typedef struct uv_getaddrinfo_s uv_getaddrinfo_t;
typedef struct uv_getnameinfo_s uv_getnameinfo_t;
typedef struct uv_shutdown_s uv_shutdown_t;
typedef struct uv_write_s uv_write_t;
typedef struct uv_connect_s uv_connect_t;
typedef struct uv_udp_send_s uv_udp_send_t;
typedef struct uv_fs_s uv_fs_t;
typedef struct uv_work_s uv_work_t;
typedef struct uv_random_s uv_random_t;
// [End libuv_request]
#endif //LIBUVDEVELOPMENT_LIBUVREQUEST_H
