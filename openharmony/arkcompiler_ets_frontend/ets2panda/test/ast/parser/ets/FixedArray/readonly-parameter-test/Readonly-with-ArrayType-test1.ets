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


function foo (p: FixedArray<int>) {
}
let x: Readonly<FixedArray<int>> = []
/* @@ label */foo(/* @@ label1 */x)

/* @@? 20:15 Error TypeError: No matching call signature for foo(readonly FixedArray<int>) */
/* @@? 20:34 Error TypeError: Type 'readonly FixedArray<int>' is not compatible with type 'FixedArray<int>' at index 1 */
