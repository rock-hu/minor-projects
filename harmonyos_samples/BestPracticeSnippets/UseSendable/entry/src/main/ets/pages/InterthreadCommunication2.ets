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
* 最佳实践: 应用并发设计 线程间通信 线程间模块共享（单例模式） （方案一）
*/
// [Start store]
import { taskpool } from '@kit.ArkTS';
import singleton from 'libentry.so';

@Concurrent
function getAddress() {
  let address = singleton.getAddress();
  console.info('taskpool:: address is ' + address);
}

@Concurrent
function store(a: number, b: number, c: number) {
  let size = singleton.getSetSize();
  console.info('set size is ' + size + ' before store');
  singleton.store(a);
  singleton.store(b);
  singleton.store(c);
  size = singleton.getSetSize();
  console.info('set size is ' + size + ' after store');
}

@Entry
@Component
struct Index {
  build() {
    Row() {
      Column() {
        Button('TestSingleton')
          .onClick(() => {
            let address = singleton.getAddress();
            console.info('host thread address is ' + address);
            let task1 = new taskpool.Task(getAddress);
            taskpool.execute(task1);
            let task2 = new taskpool.Task(store, 1, 2, 3);
            taskpool.execute(task2);
            let task3 = new taskpool.Task(store, 4, 5, 6);
            taskpool.execute(task3);
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}

// [End store]