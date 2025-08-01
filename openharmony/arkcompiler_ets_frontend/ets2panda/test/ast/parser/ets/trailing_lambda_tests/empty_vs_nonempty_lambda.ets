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

// Test empty lambda handling and parameter binding
class D {
    check(
        data: string,
        onSuccess: (d: string) => void,
        onError?: () => void  // Optional lambda last
    ) {
        onSuccess(data.toUpperCase());
        if (onError) onError();
    }
}

function testEmptyLambda() {
    const d = new D();

    // Case 1: Explicit empty lambda
    d.check("test", () => { }, () => { throw new Error() });

    // Case 2: Trailing empty lambda
    d.check("test", (d) => console.log(d)) {
        // Empty lambda binds to onError
    };

    // Case 3: Missing required lambda (should error)
    d.check("test") {
        () => { }
    };
}

/* @@? 40:5 Error TypeError: Expected 2 arguments, got 1. */
/* @@? 40:5 Error TypeError: No matching call signature for check("test") */