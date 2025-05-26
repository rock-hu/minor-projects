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
* 最佳实践: 应用切面编程设计
*/

// [Start index2]
// index2.ets
import {util} from '@kit.ArkTS';
import { Test1 } from '../components/somePackage1';

@Entry
@Component
struct Index {
  build() {
    // UI code
  }
}
// 插入执行前后打印时间, 将插入动作封装成一个接口
function addTimePrinter(targetClass: Object, methodName: string, isStatic: boolean) {
  let t1 = 0;
  let t2 = 0;
  util.Aspect.addBefore(targetClass, methodName, isStatic, () => {
    t1 = new Date().getTime();
  });
  util.Aspect.addAfter(targetClass, methodName, isStatic, () => {
    t2 = new Date().getTime();
    console.log("t2---t1 = " + (t2 - t1).toString());
  });
}
// 给Test的doSomething实例方法添加打印执行时间的逻辑
addTimePrinter(Test1, 'doSomething', false);
new Test1().doSomething()
// 给Test的test静态方法添加打印执行时间的逻辑
addTimePrinter(Test1, 'test', true);
Test1.test()
// [End index2]