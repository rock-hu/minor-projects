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

class B {
    fld: Number|undefined = 2.0;
}

function main(): void {
    let req_b: Required<B> = {fld: 2.0};
    req_b.fld = /* @@ label */undefined;
}

/* @@@ label Error TypeError: Type 'undefined' cannot be assigned to type 'Double'  */
