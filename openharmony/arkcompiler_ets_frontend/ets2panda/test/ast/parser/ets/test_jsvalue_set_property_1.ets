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

function fn(): void {
    let v: JSValue;
    let new_prop_value: JSValue;
    /* @@ label */v.prop_name = /* @@ label1 */new_prop_value;
}

/* @@@ label Error TypeError: Variable 'v' is used before being assigned.  */
/* @@@ label1 Error TypeError: Variable 'new_prop_value' is used before being assigned.  */
