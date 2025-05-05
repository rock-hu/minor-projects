/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_NAPI_TEST_NATIVE_MODULE_NETSERVER_NETSERVER_H
#define FOUNDATION_ACE_NAPI_TEST_NATIVE_MODULE_NETSERVER_NETSERVER_H

#include "event_target.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "securec.h"

#include "uv.h"

#define EVENT_TYPE_BUFFER_SIZE 64

struct WriteReq {
    uv_write_t req = { 0 };
    uv_buf_t buf = { 0 };
};

class NetServerEvent {
public:
    NetServerEvent(napi_env env, const char* type, char* buffer = nullptr, size_t length = 0)
    {
        env_ = env;

        (void)strncpy_s(type_, EVENT_TYPE_BUFFER_SIZE, type, strlen(type));

        buffer_ = buffer;
        length_ = length;
    }

    virtual ~NetServerEvent() {}

    virtual napi_value ToJsObject()
    {
        napi_value result;
        napi_create_object(env_, &result);

        napi_value type;
        napi_create_string_utf8(env_, type_, strlen(type_), &type);
        napi_set_named_property(env_, result, "type", type);

        if (length_ > 0) {
            napi_value data;
            napi_create_string_utf8(env_, buffer_, length_, &data);
            napi_set_named_property(env_, result, "data", data);
        }

        return result;
    }

private:
    napi_env env_;

    char type_[EVENT_TYPE_BUFFER_SIZE];
    char* buffer_;
    size_t length_;
};

struct NetClient {
    uv_tcp_t tcp = { 0 };
    uv_write_t req = { 0 };
    uv_buf_t buf = { 0 };
    NetClient* next = nullptr;
};

class NetServer : public EventTarget {
    NetServer(napi_env env, napi_value thisVar);
    virtual ~NetServer();

public:
    int Start(int port);
    void Stop();

    static napi_value Export(napi_env env, napi_value exports);

private:
    // Napi methods and properties
    static napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Start(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Stop(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_On(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Once(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Off(napi_env env, napi_callback_info cbinfo);

    // C function and members
    static void EchoAlloc(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf);
    static void AfterShutdown(uv_shutdown_t* req, int status);
    static void AfterWrite(uv_write_t* req, int status);
    static void AfterRead(uv_stream_t*, ssize_t nread, const uv_buf_t* buf);
    static void TakeDiffactionsByStatus(uv_stream_t* handle, ssize_t nread,
                                        const uv_buf_t* buf, NetServer* that);
    static void OnClose(uv_handle_t* peer);
    static void OnServerClose(uv_handle_t* handle);
    static void OnConnection(uv_stream_t*, int status);

    uv_loop_t* loop_;
    int serverClosed_;
    uv_tcp_t tcpServer_;
    NetClient* clients_;
};

#endif /* FOUNDATION_ACE_NAPI_TEST_NATIVE_MODULE_NETSERVER_NETSERVER_H */
