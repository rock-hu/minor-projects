/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
 * i is loop lexical
 * len is function lexical
 */
{
    let len = 7;
    function x() {
        let a = [];
        for (let i = 1; i < len; i++) {
            print("check enter loop len == 7: ", len == 7);
            if (i == 2) {
                continue;
            }

            if (i == 4) {
                break;
            }

            if (i == 6) {
                return a;
            }

            a.push(function b() {
                print(i);
            });
        }

        return a;
    }

    x().forEach(f => {
        f();
    })
}
