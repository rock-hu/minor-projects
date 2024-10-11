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

import assert from 'assert';
import path from 'path'

export function fooTestPlus(para1: number, para2: number) {
  return para1 + para2;
}

export interface Human {
  addr: string;
  age: number;
  gender: string;
  eatFood(foodName: string): void;
}

export class PersonTest1 implements Human{
  addr: string;
  age: number;
  gender: string;
  eatedFood: string[]
  constructor() {
    this.eatedFood = [];
  }
  eatFood(foodName: string): void {
    const food = foodName;
    this.eatedFood.push(food);
  }
}

const fileName: string = path.basename(__filename);
assert(fileName !== 'import_dir.ts', 'success');