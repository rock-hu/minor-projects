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

const etsVm = globalThis.gtest.etsVm;

let checkRestOfNumber = etsVm.getFunction('Lrest_spread/test/ETSGLOBAL;', 'checkRestOfNumber');
let checkRestOfString = etsVm.getFunction('Lrest_spread/test/ETSGLOBAL;', 'checkRestOfString');
let checkRestOfTuple = etsVm.getFunction('Lrest_spread/test/ETSGLOBAL;', 'checkRestOfTuple');
let checkRestofUnion = etsVm.getFunction('Lrest_spread/test/ETSGLOBAL;', 'checkRestofUnion');

function testRestOfNumber(): void {
    ASSERT_TRUE(checkRestOfNumber([1, 2, 3], 1, 2, 3));
}

function testRestOfString(): void {
    ASSERT_TRUE(checkRestOfString(['a', 'b', 'c'], 'a', 'b', 'c'));
}

function testRestOfTuple(): void {
    ASSERT_TRUE(checkRestOfTuple([[11, 'aa'], [22, 'bb'], [33, 'cc']], [11, 'aa'], [22, 'bb'], [33, 'cc']));
}

function testRestofUnion(): void {
    ASSERT_TRUE(checkRestofUnion([2, 3, true, 'a', 'b'], 2, 3, true, 'a', 'b'));
}

testRestOfNumber();
testRestOfString();
testRestOfTuple();
testRestofUnion();