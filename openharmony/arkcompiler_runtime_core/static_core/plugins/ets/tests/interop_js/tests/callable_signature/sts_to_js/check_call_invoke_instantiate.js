/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

const {
    jsInt,
    jsString,
    InvokeClass,
    InstantiateClass
} = require('callable_signature.test');


function checkCallInvokeClass() {
    try {
        InvokeClass(jsInt, jsInt);

        ASSERT_TRUE(false);
    } catch (e) {
        ASSERT_TRUE(true);
    }
}

function checkCallInstantiateClass() {
    try {
        InstantiateClass(jsString);

        ASSERT_TRUE(false);
    } catch (e) {
        ASSERT_TRUE(true);
    }
}

checkCallInvokeClass();
checkCallInstantiateClass();
