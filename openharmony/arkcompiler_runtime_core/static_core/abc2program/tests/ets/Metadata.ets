/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

@interface Anno {
  name: string
}

interface Iface {
  bar()
}

@Anno({
  name: "Cls"
})
class Cls implements Iface {
  @Anno({
      name: "foo"
  })
  foo: int = 1
  @Anno({
      name: "bar"
  })
  bar() {
    console.log("bar")
  }
}

namespace Ns {
  function add(num1: int, num2: int): int {
    return num1 + num2;
  }
}