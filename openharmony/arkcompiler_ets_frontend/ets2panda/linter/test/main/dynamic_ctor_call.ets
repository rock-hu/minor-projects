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

function FunctionCtor() {}
let c = new FunctionCtor();

interface I {}

type IConstructor = new (x: number, y: number) => I;

function createI(
  ctor: IConstructor
): I {
  return new ctor(1, 2);
}

function createI2(
  ctor: new (x: number, y: number) => I
): I {
  return new ctor(1, 2);
}

interface IConstructor3 {
  new (s: string): I;
}
function createI3(ctor: IConstructor3) {
  return new ctor("hello");
}

class C {}

type Constructor = {
  new (s: string): C;
};

function createC(ctor: Constructor) {
  return new ctor("hello");
}
