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
 * i && j are loop lexical
 * len is function lexical
 */
{
    let a = [];
    let len = 6;

    function x() {
        return len;
    }

    for (let i = 1; i < 6; i++) {
        print("check enter loop, len == 6: ", len == 6);
        let j = i;
        if (i == 3) {
            continue;
        }

        if (i == 5) {
            break;
        }

        a.push(function b() {
            print(i, j);
        });
    }

    print("check exit loop, len == 6: ", len == 6);

    a.forEach(f => {
        f();
    })
}
