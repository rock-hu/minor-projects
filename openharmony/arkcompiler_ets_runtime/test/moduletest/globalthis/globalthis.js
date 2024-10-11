/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * Instructions:
 * The case is extracted from application, had occured a IC LoadMiss bug,
 * when the op is not found, should not generate cache.
 */

/*
 * @tc.name:globalthis
 * @tc.desc:test global this
 * @tc.type: FUNC
 * @tc.require: issueI5NO8G
 */
let res;
for (let i = 0; i < 100; ++i) {
    res = globalThis.a;

    if (i == 50) {
        globalThis.a = 2;
    }
}
print(res);
print(Object.keys(globalThis))