/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

export class DependencyClass {
  depencyProp1: string = "";
  depencyProp2: number = 1;
  depencyMethod1() {
    const tempValue = 2;
    return this.depencyProp1 + this.depencyProp2;
  }
}

export function DependencyFunctionReduce(num1: number, num2: number) {
  return num1 - num2;
}


export interface DependencyInterface {
  depencyProp3: string;
  depencyProp4: number;
}