/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

/*
 * @tc.name:allocatesizeoverflow
 * @tc.desc:test allocatesizeoverflow function
 * @tc.type: FUNC
 */
const v0 = [746660.144619491,-3.13292893636182e+307,530.966841143204,2.2250738585072014e-308,-1.5938296745618066e+308,NaN];
v0.length = 2465979003;
try {
    isNaN.apply(undefined, v0);
} catch (error) {
    print(error);
}
