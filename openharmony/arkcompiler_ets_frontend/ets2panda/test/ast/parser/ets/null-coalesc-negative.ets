/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

class A {
  protected count: int;
  getCount(): int {
    return this.count
  }
}

class B extends A {
  increment(): void {
    this.count++;
  }
}

class C extends A {
}

function main(): void {
  let b = new B();
  let c = new C();
  b.increment();
  let a = b ?? c;
  /* @@ label */a./* @@ label1 */increment();
}

/* @@@ label1 Error TypeError: Property 'increment' does not exist on type 'C'  */
/* @@@ label Error TypeError: Member type must be the same for all union objects.  */
