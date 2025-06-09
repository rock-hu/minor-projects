/**
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
let etsVm = requireNapiPreview('ets_interop_js_napi', true);

let etsVmRes = etsVm.createRuntime({
    'load-runtimes': 'ets',
    'boot-panda-files': 'etsstdlib.abc',
    'gc-type': 'epsilon'
});
if (etsVmRes) {
    throw new Error('Test failed: expected runtime cannot be created with GC other than G1');
}

etsVmRes = etsVm.createRuntime({
    'load-runtimes': 'ets',
    'boot-panda-files': 'etsstdlib.abc',
    'no-async-jit': 'true'
});
if (etsVmRes) {
    throw new Error('Test failed: expected runtime cannot be created with "no-async-jit" option');
}

