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

export let var1 = 1;
export let var2 = 2;
export let var3 = 3;

export function addFunc(para1: number, para2: number){
  return para1 + para2;
}
export function reduceFunc(para1: number, para2: number){
  return para1 - para2;
}

export default function maxFunc(num1: number, num2: number) {
  return Math.max(num1, num2);
}
