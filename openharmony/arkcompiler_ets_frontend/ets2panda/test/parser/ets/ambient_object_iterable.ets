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

declare class CIterator implements Iterator<string> {

  public ind_: int;
  public base_: C;

  constructor (base: C)

  override next(): IteratorResult<string>
}

declare class C {
  protected y: string[];

  [Symbol.iterator](): CIterator
}

function main(): void {

  let c = new C();
  let i: int = 0;

  for (let it of c) {
    it += ": in for";

    if (i == 0) {
      assertEQ(it, "one,  in for");
    } else if (i == 1) {
      assertEQ(it, "two,  in for");
    } else if (i == 2) {
      assertEQ(it, "three,  in for");
    } else {
      assertTrue(false);
    }

    ++i;
  }
}