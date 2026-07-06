/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
// [Start Interceptor]
// Interceptor.ets
import { AxiosResponse, InternalAxiosRequestConfig } from '@ohos/axios';
import { IRequestInterceptor, IResponseInterceptor } from './AxiosConfig';

// Request interceptor encapsulated as sendable
@Sendable
export class RequestInterceptor implements IRequestInterceptor {
  handle(data: InternalAxiosRequestConfig<object>): InternalAxiosRequestConfig<object> |
    Promise<InternalAxiosRequestConfig<object>> {
    return data;
  }
  handleError(error: object): object {
    return error;
  }
}

// Response interceptor encapsulated as sendable
@Sendable
export class ResponseInterceptor implements IResponseInterceptor {
  handle(data: AxiosResponse<object, object>): AxiosResponse<object, object> |
    Promise<AxiosResponse<object, object>> {
    return data;
  }
  handleError(error: object): object {
    return error;
  }
}
// [End Interceptor]