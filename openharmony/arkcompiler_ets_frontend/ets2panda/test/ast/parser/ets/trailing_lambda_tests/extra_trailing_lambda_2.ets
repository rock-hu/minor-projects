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

// Test excess trailing lambdas when parameters are full
class C {
    process(
        required?: number,
        optional?: string,
        callback: () => void = () => { },
    ) { }
}

function testExtraTrailing() {
    const c = new C();

    // Case 1: Normal call with all parameters
    c.process(1, "ok", () => { });

    // Case 2: Excess trailing lambda (should error)
    c.process(1, "ok", () => { }) {
        console.log("extra");
    };
}

/* @@? 32:5 Error TypeError: No matching call signature with trailing lambda */
