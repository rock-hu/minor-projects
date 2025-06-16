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
class A {}
let a1 = new A();
let a2 = new A();

function f(a) {
    ArkTools.forceLazyDeopt(a, 2, false);
    a.x = 1;
    print(a.x);
}

f(a1);
ArkTools.jitCompileAsync(f);
print(ArkTools.waitJitCompileFinish(f));

print("------------------------------------------------------");
f(a2);