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
import { module1 } from './export_namespace_export_01';
import defaultApi, {var1, reduce, Human, plus, Person, defaultAlias} from './export_name_export';

assert(module1.reduce(3, 1) === 2);
assert(defaultApi([2, 4, 6]) === 6);
assert(var1 === 1);
assert(reduce(4,1) === 3);
let ins = new Human();
assert(ins.getAge === 1);
assert(plus(4,1) === 5);
let ins2 = new Person(3);
assert(ins2.doubleProp1() === 6);
assert(defaultAlias([1, 2, 3, 4, 5, 6]).toString() === "2,4,6");

