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

export let foo = {isGood: true}
export let ff1 = {f1: 10}
export let ff2 = {f2: 15}

export class A {}
export class C {}

export let ff3 = {arr: [1, 2, 3, 4, 5, 6, 7, 8, 9]}

export function ff4() {
  throw 123
}

export function handle(cb) {
  let p  = {name: 'hello'}
  cb(p)
}

export function expand(obj) {
  let x = obj;
  let {a, b, c} = obj;
}

export let orange = {
  isFruit: true,
  isVegetable: false,
  isVegetable1: 123
}
