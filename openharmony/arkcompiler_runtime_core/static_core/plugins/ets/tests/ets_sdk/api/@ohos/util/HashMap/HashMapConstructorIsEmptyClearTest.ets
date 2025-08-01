/*
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

import HashMap from "@ohos.util.HashMap";

function main(): int {
    const suite = new ArkTestsuite("HashMap Constructor IsEmpty Clear API tests")

    suite.addTest("HashMap construct create default empty hashMap", testConstructorCreatesEmptyHashMap)
    suite.addTest("HashMap construct create hashMap add custom type data", testConstructorWithCustomTypeData)
    suite.addTest("HashMap isEmpty is true", testIsEmptyTrue)
    suite.addTest("HashMap isEmpty is false", testIsEmptyFalse)
    suite.addTest("HashMap isEmpty is true when remove the data", testIsEmptyAfterRemovingData)
    suite.addTest("HashMap isEmpty is true when clear the data", testIsEmptyAfterClearingData)
    suite.addTest("HashMap clear the data", testClearData)
    suite.addTest("HashMap clear the null hashMap", testClearEmptyHashMap)
    suite.addTest("HashMap clear data and set data again", testClearAndSetDataAgain)

    return suite.run()
}

function testConstructorCreatesEmptyHashMap() {
    let hashMap: HashMap<string, number> = new HashMap<string, number>();
    hashMap.set("squirrel", 123);
    assertTrue(hashMap instanceof HashMap, "The hashMap should be of the same type as the HashMap");
    assertEQ(hashMap.length, 1, "The hashMap length should be 1");
}

class Tmp {
    name: number = 0
}
function testConstructorWithCustomTypeData() {
    let hashMap: HashMap<string, Tmp> = new HashMap<string, Tmp>();
    let param: Tmp = {
        name: 1
    };
    hashMap.set("squirrel", param);
    let result = hashMap.hasValue(param);
    assertTrue(hashMap instanceof HashMap, "The hashMap should be of the same type as the HashMap");
    assertEQ(result, true, "The hashMap contains the specified value");
}

function testIsEmptyTrue() {
    let hashMap: HashMap<string, number> = new HashMap<string, number>();
    let result = hashMap.isEmpty();
    assertEQ(result, true, "The hashMap should be empty");
    assertEQ(hashMap.length, 0, "The hashMap length should be 0");
}

function testIsEmptyFalse() {
    let hashMap: HashMap<string, number> = new HashMap<string, number>();
    hashMap.set("squirrel", 123);
    let result = hashMap.isEmpty();
    assertEQ(result, false, "The hashMap should not be empty");
    assertEQ(hashMap.length, 1, "The hashMap length should be 1");
}

function testIsEmptyAfterRemovingData() {
    let hashMap: HashMap<string, number> = new HashMap<string, number>();
    hashMap.set("squirrel", 123);
    let result1 = hashMap.isEmpty();
    hashMap.remove("squirrel")
    let result2 = hashMap.isEmpty();
    assertEQ(result1, false, "The hashMap should not be empty");
    assertEQ(result2, true, "The hashMap should be empty");
    assertEQ(hashMap.length, 0, "The hashMap length should be 0");
}

function testIsEmptyAfterClearingData() {
    let hashMap: HashMap<string, number> = new HashMap<string, number>();
    hashMap.set("squirrel", 123);
    let result1 = hashMap.isEmpty();
    hashMap.clear()
    let result2 = hashMap.isEmpty();
    assertEQ(result1, false, "The hashMap should not be empty");
    assertEQ(result2, true, "The hashMap should be empty");
    assertEQ(hashMap.length, 0, "The hashMap length should be 0");
}

function testClearData() {
    let hashMap: HashMap<string, number> = new HashMap<string, number>();
    hashMap.set("squirrel", 123);
    hashMap.clear();
    let result = hashMap.get("squirrel");
    assertEQ(result, undefined, "The hashMap should be cleared");
    assertEQ(hashMap.length, 0, "The hashMap length should be 0");
}

function testClearEmptyHashMap() {
    let hashMap: HashMap<string, number> = new HashMap<string, number>();
    hashMap.clear();
    assertEQ(hashMap.length, 0, "Empty HashMap should remain empty after clear");
}

function testClearAndSetDataAgain() {
    let hashMap: HashMap<string, number> = new HashMap<string, number>();
    hashMap.set("ji", 12);
    hashMap.clear();
    hashMap.set("ji", 13);
    let result = hashMap.get("ji");
    assertEQ(result, 13, "The hashMap should return the correct value for the key");
    assertEQ(hashMap.length, 1, "The hashMap length should be 1");
}
