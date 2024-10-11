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

class BasicClass {
  private __name: string;

  constructor(__name: string) {
    this.__name = __name;
  }
}
  
class TestClass3 {
  constructor(public __publicParam: number, private __privateParam: string, protected __protectedParam: number) {
    this.__publicParam = 1;
    this.__privateParam = "123";
    this.__protectedParam = 1;
  }
}
  