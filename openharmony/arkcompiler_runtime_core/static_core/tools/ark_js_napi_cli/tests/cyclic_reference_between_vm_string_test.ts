/**
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

// This test check that ETS array with large string
// that is transferred from ETS -> JS and then from JS -> ETS
// creating cyclic reference - can be collected by GC

import { interop } from './gc_test_common';

function main(): void {
    for (let i = 0; i < 2; i++) {
        let arr: Object[] = [];
        for (let j = 0; j < 2; j++) {
            arr.push(interop.GetSTSStringArrayAsObject());
            interop.RunInteropGC();
            interop.RunPandaGC();
            globalThis.test.RunJsGC();
        }
        interop.RunInteropGC();
        interop.RunPandaGC();
        globalThis.test.RunJsGC();
        interop.AddPandaArray(arr);
    }
}
main();
