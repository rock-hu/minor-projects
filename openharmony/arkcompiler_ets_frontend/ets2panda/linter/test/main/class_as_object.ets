/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

import { Something, SomethingFactory, SomethingBar, Bar, Select } from "./oh_modules/ohos_factory";

class C {
    static a = 5
    static b = 8
}

namespace H {
    export class G {}
}

let c = C
c = C
let g = H.G
g = H.G

let ca = C.a
let cb = C.b

let cc = new C()
let gg = new H.G()

function foo1(arg: typeof C) {}
foo1(C)

function foo2(arg: C) {
    return C
}

function foo3(arg: typeof H.G) {}
foo3(H.G)

function foo4(arg: H.G) {
    return H.G
}

class A {}
interface B {}
interface I {}
class CC extends A implements I, B {}

class D {
    constructor(arg: typeof C) {}
}
new D(C)

type X = D;

namespace test1 {
    class SomethingFoo extends Something { }
    namespace NS { export class SomethingFoo extends Something { } }

    let fact = SomethingFactory.getInstance();

    let x1 = fact.create1(SomethingFoo).beep();
    let x2 = fact.create1(SomethingBar).beep();
    let x3 = fact.create1(NS.SomethingFoo).beep();

    let x4 = fact.create2({ o: SomethingFoo });
    let x5 = fact.create2({ o: SomethingBar });
    let x6 = fact.create2({ o: NS.SomethingFoo });

    let x7 = fact.create3(() => SomethingFoo);
    let x8 = fact.create4(() => SomethingFoo);

    let x9 = new Bar(SomethingFoo);
}

enum Color { WHITE, BLUE, RED };
let color: Color = Color.RED;

for (let item = 0; item < Object.keys(Color).length; item++) {
    console.log(item);
}

foo2(() => C);

export { C as H };

// #14228
let data = new Select<Object>().from(C).eq('key').query(C); // Ok
invalid_func(C); // Ok
let a: any;
a.foo(C); // Ok

let col = 'WHITE';
console.log(Color[col])

// #14184
namespace NS {
    export enum E {
        A = 'A',
        B = 'B',
        C = 'C'
    }
}

let s: string = 'B';
let s2: string = NS.E[s];

for (let item = 0; item < Object.keys(NS.E).length; item++) {
    console.log(item);
}

/**
 * {@link C} - should not report error
 */
class JSDocClass {}

function classAsObject(x: any): void {}

classAsObject(Object);
classAsObject(Function);
classAsObject(String);
classAsObject(Boolean);
classAsObject(Number);
classAsObject(Math);
classAsObject(Date);
classAsObject(RegExp);
classAsObject(Error);
classAsObject(EvalError);
classAsObject(RangeError);
classAsObject(ReferenceError);
classAsObject(SyntaxError);
classAsObject(TypeError);
classAsObject(URIError);
classAsObject(AggregateError);
classAsObject(JSON);
classAsObject(Array);
classAsObject(ArrayBuffer);
classAsObject(DataView);
classAsObject(Int8Array);
classAsObject(Uint8Array);
classAsObject(Uint8ClampedArray);
classAsObject(Int16Array);
classAsObject(Uint16Array);
classAsObject(Int32Array);
classAsObject(Uint32Array);
classAsObject(Float32Array);
classAsObject(Float64Array);
classAsObject(Map);
classAsObject(WeakMap);
classAsObject(Set);
classAsObject(WeakSet);
classAsObject(Promise);
classAsObject(Proxy);
classAsObject(Symbol);
classAsObject(SharedArrayBuffer);
classAsObject(Atomics);
classAsObject(BigInt);
classAsObject(BigInt64Array);
classAsObject(BigUint64Array);
classAsObject(WeakRef);
classAsObject(FinalizationRegistry);

const aggrErr = AggregateError([new Error()]);
const array = Array<number>(10);
const bi = BigInt(10);
const bool = Boolean('true');
const date = Date();
const err = Error('message');
const evalErr = EvalError('message');
const func = Function('console.log(1)');
const num = Number('10');
const obj = Object();
const rangeErr = RangeError('message');
const refErr = ReferenceError('message');
const regex = RegExp('abc');
const str = String('value');
const sym = Symbol('symbol1');
const syntaxErr = SyntaxError('message');
const typeErr = TypeError('message');
const uriErr = URIError('message');

let aaa4 = typeof DataView;

let obj = new Object();
switch (obj) {
  case Object:
    console.log('A');
    break;
  default:
    console.log('F');
}

function parse(type: "number" | "boolean", value: string): number | boolean {
  return type === "number" ? Number(value) : value === "true";
}
function format(input: string | number): string[] | number[] {
  return typeof input === "string" ? input.split("") : input.toString().split("").map(Number);
}

let version: string = "1.2"
let a1 = version.split('.').map(Number);

let version2: string = "1.2"
let a2 = version2.split('.').map(String);

let version3: string = "1.2"
let a3 = version3.split('.').map(Boolean);