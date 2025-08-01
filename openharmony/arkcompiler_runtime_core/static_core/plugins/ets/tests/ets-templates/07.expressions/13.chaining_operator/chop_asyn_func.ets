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

/*---
desc: Chaining operator, call an asynchronous function that may be undefined.
---*/

class Person {
  name: string;
  fetchData?: () => Promise<string|undefined>;

  constructor(name: string) {
    this.name = name;
  }
}

async function test(): Promise<void> {
  const bob = new Person("Bob");
  const resultPromise = bob.fetchData?.() ?? Promise.resolve(undefined);
  const result = await resultPromise;
  assertEQ(result, undefined);
}

function main(): void {
  test();
}