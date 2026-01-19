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
// [Start AxiosConfig]
// AxiosConfig.ets
import { collections, lang } from '@kit.ArkTS';
import { AxiosResponse, InternalAxiosRequestConfig } from '@ohos/axios';

// Transforming the Request Interceptor into a Sendable interface
export interface IRequestInterceptor extends lang.ISendable {
  handle(data: InternalAxiosRequestConfig<object>): InternalAxiosRequestConfig<object> |
    Promise<InternalAxiosRequestConfig<object>>;
  handleError(error: object): object;
}

// Transforming the Response Interceptor into a Sendable interface
export interface IResponseInterceptor extends lang.ISendable {
  handle(data: AxiosResponse<object, object>): AxiosResponse<object, object> |
    Promise<AxiosResponse<object, object>>;
  handleError(error: object): object;
}

// Transforming the Auth into a Sendable interface
export interface IAuth extends lang.ISendable {
  username: string;
  password: string;
}

// Transforming the HttpProxy into a Sendable interface
export interface IHttpProxy extends lang.ISendable {
  protocol: string;
  host: string;
  port: number;
  auth: IAuth;
  exclusionList: collections.Array<string>;
}

// Encapsulate Axios configuration, which needs to be initialized on the main thread
@Sendable
export class AxiosGlobalConfig {
  private constructor() {
    this.requestInterceptors = new collections.Array<IRequestInterceptor>();
    this.responseInterceptors = new collections.Array<IResponseInterceptor>();
    this.headers = new collections.Map<string, string>();
  }

  public baseURL?: string;
  public headers: collections.Map<string, string>;
  public timeout?: number;
  public proxy?: IHttpProxy;
  public xsrfCookieName?: string;
  public xsrfHeaderName?: string;
  public maxContentLength?: number;
  public maxBodyLength?: number;
  // [StartExclude AxiosConfig]
  // [EndExclude AxiosConfig]

  public requestInterceptors: collections.Array<IRequestInterceptor>;
  public responseInterceptors: collections.Array<IResponseInterceptor>;
  public static instance: AxiosGlobalConfig = new AxiosGlobalConfig();
}
// [End AxiosConfig]