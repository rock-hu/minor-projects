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

import {lang} from './@arkts.lang';
import {II as III} from "./shared_module_sendable_export";
'use shared'

export const enum A{a, b} // ok, const enum is sendable type
export enum B{a, b} // error, enum is not sendable type
export let var1: number | A; // ok, union type of const enum and number
export let var2: A.a; // error, enum member is not sendable type

export class C{} // error, class is not sendable type
@Sendable
export class D{} // ok, sendable class is sendable type
export let var3: A | C; //error

export interface E{} // error, interface is not sendable type
export interface F extends lang.ISendable{} // ok, interface extends ISendable is sendable type; 
type ff = F;
type ee = E;
export {ff}; // ok
export {ee}; // error
let var4: C | ee;
let var5: D | ff;
export {var4}; // error
export {var5}; // ok

export * as ns from './shared_module_sendable_export'; // err,  ns is not sendable type
export * from './shared_module_unsendable_export'; // error, 'export * from ...' is not supported
export default III; // ok
export {a} from './shared_module_sendable_export'; // ok
export {a as aa} from './shared_module_unsendable_export'; // error
export let var6: A | A.a; // error, union type of sendable and unsendbale
export let var7: string | B = 'aaa'; // error , union type of sendable and unsendbale
export let var8 = var7; // ok, no explicit type, the inferred type of var8 is string
namespace ns {
    export class A{}  // ok, exports from namespace are not checked 
}

function foo(): boolean {
    return true;
}

export {foo};

@Sendable
export function sf():void {
}

@Sendable
function sf2():void {
}

export {sf2};

export const primitive_str = 'a'; // OK
export const primitive_num = 1; // OK
export const primitive_bool = true; // OK
export const primitive_big = 1n; // OK

export enum NormalEnum { // ERROR,
  a,
  b,
  c,
  d
}

export const enum ConstEnum { // OK
  a,
  b,
  c,
  d
}

export const unite1 = // ERROR
  Math.random() < 0.5
    ? NormalEnum.a
    : primitive_str;

export const unite2 = Math.random() < 0.5 ? primitive_str : primitive_big; // OK

export {
  primitive_str as strAlias,
  primitive_num as numAlias,
  primitive_bool as boolAlias,
  primitive_big as bigAlias,
  NormalEnum as NormalEnumAlas // ERROR
};

export type TypeC = C; // ERROR
export type TypeE = E; // ERROR
