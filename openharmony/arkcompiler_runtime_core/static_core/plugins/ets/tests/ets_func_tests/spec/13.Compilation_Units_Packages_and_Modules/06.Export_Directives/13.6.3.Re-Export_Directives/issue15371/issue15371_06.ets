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

/*---
desc: 13.compilation_units\13.06.export_directives\13.06.03.re-export_directives\issue15371
name: issue15371_06.ets
files: [./issue15371_03.ets]
---*/

import { boo } from './issue15371_03'
import { bar } from './issue15371_03'

function main(): void {
  assertEQ( boo(), 0)
  assertEQ( bar(3), 9)
}
