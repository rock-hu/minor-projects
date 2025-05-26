/*
* Copyright (C) 2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/*
* 最佳实践: 应用并发设计 线程间通信 跨语言多线程通信（C++与ArkTS）
*/
// [Start inter_thread_communication1]
// xxx.ets
import nativeModule from 'libentry.so';

@Entry
@Component
struct InterthreadCommunication1 {
  build() {
    Button('click me')
      .onClick(() => {
        nativeModule.nativeCall((a: number) => {
          console.log('Received data from thread-function: %{public}d', a);
        })
      })
  }
}
// [End inter_thread_communication1]