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
desc: >-
    If there is a cyclic dependency between top-level variable declarations, then a
    compile-time error occurs.
    Module A imports module B, which imports module C. Module C imports module A.
tags: [compile-only, negative,not-a-test]
files:
- ./module_a.ets
---*/

import {a} from './module_a.ets'

export let c: number = 3;
