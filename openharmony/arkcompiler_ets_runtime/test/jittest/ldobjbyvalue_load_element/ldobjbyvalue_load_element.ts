/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

function test(str, index) {
    return str[index] == '0';
}

let str = "adas0s";
for (let i = 0; i < 1; i++) {
    test(str, 4);
    test(str, '4');
}

ArkTools.jitCompileAsync(test);
print(ArkTools.waitJitCompileFinish(test));
print(test(str, 4));
print(test(str, '4'));
