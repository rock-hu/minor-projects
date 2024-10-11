/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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


// 3. non-static accessors can be named with a private identifier, and can't call it outside the class

class C3 {
  #value: number = 0;
  get data() {
    return this.#value;
  }
  set #data(num: number) {
    this.#value = num;
  }
}

let c3: C3 = new C3();
c3.data = 20;
print(c3.#data)
