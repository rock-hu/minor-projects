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

class B {
    name_: string
}

function name(b: B) {}
function name(b: B, n: string) {}

get name/* @@ label1 */(this: B): string {
    return this.name_; 
}

set name/* @@ label2 */(this: B, n: string) {
    this.name_ = n;
}

/* @@@ label1 Error TypeError: Function name with this assembly signature already declared.  */
/* @@@ label2 Error TypeError: Function name with this assembly signature already declared.  */
