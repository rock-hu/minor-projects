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
// [Start axios]
// Sample7.ets
import { taskpool } from '@kit.ArkTS';
import { AxiosError, AxiosResponse } from '@ohos/axios';
import { getAxios } from './AxiosAdapter';
import { AxiosGlobalConfig, IRequestInterceptor, IResponseInterceptor } from './AxiosConfig';
import { RequestInterceptor, ResponseInterceptor } from './Interceptor';

function initAxios(url: string): void {
  // init axios config
  AxiosGlobalConfig.instance.baseURL = url;
  let requestInterceptors: IRequestInterceptor = new RequestInterceptor();
  let responseInterceptors: IResponseInterceptor = new ResponseInterceptor();
  AxiosGlobalConfig.instance.requestInterceptors.push(requestInterceptors);
  AxiosGlobalConfig.instance.responseInterceptors.push(responseInterceptors);
  AxiosGlobalConfig.instance.timeout = 1000;
}

@Concurrent
function testAxios(config: AxiosGlobalConfig) {
  // taskpool function
  AxiosGlobalConfig.instance = config;
  let adapter = getAxios();
  adapter.get(AxiosGlobalConfig.instance.baseURL).then((res: AxiosResponse) => {
    console.log('testAxios: ' + JSON.stringify(res.data));
  }).catch((error: AxiosError) => {
    console.error('error: ' + error.message);
  });
}

function demo() {
  let url = ''; // internet url
  initAxios(url);
  for (let i = 0; i < 5; i++) {
    // TaskPool thread use Axios
    let task: taskpool.Task = new taskpool.Task(testAxios, AxiosGlobalConfig.instance);
    taskpool.execute(task);
  }
}

@Entry
@Component
struct Sample7 {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column({ space: 12 }) {
        Button('test')
          .height(40)
          .width('100%')
          .onClick(() => {
            demo();
          })
      }
      .height('100%')
      .width('100%')
      .padding(16)
      .justifyContent(FlexAlign.Center)
    }
    .height('100%')
  }
}
// [End axios]