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

import A from "./eitest_export_import_default_A_B_1"

function main(){
    let a :A = new A()
}

/* @@? 16:15 Error TypeError: Cannot find default imported element in the target */
/* @@? 19:12 Error TypeError: Cannot find type 'A'. */
/* @@? 19:20 Error TypeError: Cannot find type 'A'. */