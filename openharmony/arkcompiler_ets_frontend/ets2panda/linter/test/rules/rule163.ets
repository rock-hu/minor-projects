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

// a.ets
'use shared'
export enum E { A, B } // Error, regular enum is not sendable
export class C {} // Error, class C is not sendable
export let v1: C; // Error, v1 has a non-sendable type
type T1 = C;
export { T1 }; // Error, type T1 is aliasing the non-sendable type
let v2: T1;
export { v2 }; // Error, v2 has a non-sendable type
export { D } from 'b'; // Error, re-exporting non-sendable class
export { v3 } from 'b'; // Error, re-exporting variable with non-sendable type

// b.ets
export class D {}
export let v3: D;

