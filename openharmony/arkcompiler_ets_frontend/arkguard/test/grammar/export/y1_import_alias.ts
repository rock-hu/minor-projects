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

export * as CDE11 from './y1_export_alias';
export { default } from './y1_export_alias';
import * as CDE22 from './y1_export_alias';

let ins = new CDE22.ClassAlis;
assert(ins.classProp1 === 2, 'success');
type typeAlis = CDE22.InterOriginal2;
assert(CDE22.fooOriginal2() === '11', 'success');

