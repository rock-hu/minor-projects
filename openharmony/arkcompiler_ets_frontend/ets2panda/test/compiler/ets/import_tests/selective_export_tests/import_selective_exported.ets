/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

import * as all from "./selective_export"
import {func} from "./selective_export";

function main(): void {
    func();
    all.afunc();
    console.print(all.msg);
    console.print(all.amsg);

    let test_var: all.tc = new all.tc();
    console.print(test_var.test_member);
    test_var.test_method();

    let alias_var = new all.aliased();
    alias_var.alias_class_method();
}
