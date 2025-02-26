/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 * @tc.name:proxy
 * @tc.desc:test proxy
 * @tc.type: FUNC
 * @tc.require: issueIBCUJM
 */

// This case aims to test if there's stack-overflow checking in RuntimeOptConstructProxy
{
    let v0 = new Proxy(function () {}, {});
    for (let v1 = 0; v1 < 10000; v1++) {
        v0 = new Proxy(v0, {});
    }
    let error = new Error();
    try {
        new v0(0);
    } catch (e) {
        error = e;
    }
    assert_equal(error.name, "RangeError");
    assert_equal(error.message, "Stack overflow!");
}

test_end();
