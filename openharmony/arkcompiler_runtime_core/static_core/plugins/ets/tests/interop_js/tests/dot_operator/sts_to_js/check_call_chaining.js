
/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

const {
    jsInt,
    CheckDotClass,
} = require('dot_operator.test');

const Obj = new CheckDotClass();

function checkCheckCallChaining() {
    const expectResult = -5;
    const resChaining = Obj.arr.reverse().map(i => i + jsInt).filter(i => i > jsInt).sort().reduce((a, b) => a - b);

    ASSERT_TRUE(resChaining === expectResult);
}

checkCheckCallChaining();
