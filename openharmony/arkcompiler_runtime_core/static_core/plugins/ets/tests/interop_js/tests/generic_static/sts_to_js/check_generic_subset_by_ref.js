/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const {
    getClass,
    getFunction,
    jsArrInt,
    checkArray,
} = require('generic_static.test');

const SubsetByRefStatic = getClass('SubsetByRefStatic');
const genericSubsetByRefStaticCallFromSts = getFunction('genericSubsetByRefStaticCallFromSts');

function checkSubsetByRefStatic() {
    const res = SubsetByRefStatic.get(jsArrInt);

    ASSERT_TRUE(checkArray(res) && res[0] === jsArrInt[0]);
}

function checkGenericSubsetByRefStaticCallFromSts() {
    const res = genericSubsetByRefStaticCallFromSts(jsArrInt);

    ASSERT_TRUE(checkArray(res) && res[0] === jsArrInt[0]);
}

checkSubsetByRefStatic();
checkGenericSubsetByRefStaticCallFromSts();
