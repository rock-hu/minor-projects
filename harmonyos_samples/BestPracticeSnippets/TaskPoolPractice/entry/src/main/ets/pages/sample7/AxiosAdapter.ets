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
// [Start AxiosAdapter]
// AxiosAdapter.ets
import axios, { Axios, AxiosInstance, AxiosProxyConfig, AxiosResponse, InternalAxiosRequestConfig } from '@ohos/axios';
import { AxiosGlobalConfig } from './AxiosConfig';

// Initialize axios object according to configuration and return the object.
export function getAxios(): Axios {
  let instance: AxiosInstance = axios.create();
  if (AxiosGlobalConfig.instance.baseURL) {
    instance.defaults.url = AxiosGlobalConfig.instance.baseURL;
  }
  for (let entry of AxiosGlobalConfig.instance.headers.entries()) {
    instance.defaults.headers[entry[0]] = entry[1];
  }
  if (AxiosGlobalConfig.instance.timeout) {
    instance.defaults.timeout = AxiosGlobalConfig.instance.timeout;
  }
  if (AxiosGlobalConfig.instance.proxy) {
    let config: AxiosProxyConfig = {
      host: AxiosGlobalConfig.instance.proxy.host,
      port: AxiosGlobalConfig.instance.proxy.port,
      exclusionList: []
    };
    instance.defaults.proxy = config;
  }
  for (let interceptor of AxiosGlobalConfig.instance.requestInterceptors.values()) {
    axios.interceptors.request.use((config: InternalAxiosRequestConfig<object>): InternalAxiosRequestConfig<object> |
      Promise<InternalAxiosRequestConfig<object>> => interceptor.handle(config),
      (error: object) => interceptor.handleError(error));
  }
  for (let interceptor of AxiosGlobalConfig.instance.responseInterceptors.values()) {
    axios.interceptors.response.use((response: AxiosResponse<object, object>): AxiosResponse<object, object> |
      Promise<AxiosResponse<object, object>> => interceptor.handle(response),
      (error: object) => interceptor.handleError(error));
  }
  return axios;
}
// [End AxiosAdapter]