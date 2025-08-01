/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
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

import {ArrayList} from "@ohos.util.ArrayList";

function main() {
    const suite = new ArkTestsuite("ArrayList API tests");

    suite.addTest("ArrayList replaceAllElements with constant value", testReplaceAllElementsWithConstantValue);
    suite.addTest("ArrayList replaceAllElements with lambda", testReplaceAllElementsWithLambda);
    suite.addTest("ArrayList replaceAllElements with lambda and index", testReplaceAllElementsWithLambdaAndIndex);
    suite.addTest("ArrayList replaceAllElements with lambda, index and tempArrayList",
        testReplaceAllElementsWithLambdaIndexAndTempArrayList);
    suite.addTest("ArrayList forEach with no parameter function", testForEachNoParameterFunction);
    suite.addTest("ArrayList forEach with function with value", testForEachFunctionWithValue);
    suite.addTest("ArrayList forEach with function with value and index", testForEachFunctionWithValueAndIndex);
    suite.addTest("ArrayList forEach with function with value, index and tempArrayList",
        testForEachFunctionWithValueIndexAndTempArrayList);
    suite.addTest("ArrayList forEach with change the elements", testForEachChangeElements);

    return suite.run();
}

class TestData {
    static readonly testNumber1: number = 1;
    static readonly testNumber3: number = 3;
    static readonly testNumber5: number = 5;
    static readonly testNumber12: number = 12;
    static readonly forNumber: Number[] = [1, 2, 3, 4, 5];
}

function testReplaceAllElementsWithConstantValue() {
    let arrayList = new ArrayList<number>();
    initializeArrayList(arrayList, TestData.forNumber);
    arrayList.replaceAllElements((): number => {
        return TestData.testNumber3;
    });
    for (let i: number = 0; i < arrayList.length; i++) {
        assertEQ(arrayList[i], TestData.testNumber3, `Element at index ${i} should match`);
    }
}

function testReplaceAllElementsWithLambda() {
    let arrayList = new ArrayList<number>();
    initializeArrayList(arrayList, TestData.forNumber);
    arrayList.replaceAllElements((value: number): number => {
        return value + TestData.testNumber3;
    });
    for (let i: int = 0; i < arrayList.length; i++) {
        assertEQ(arrayList[i], TestData.forNumber[i] + TestData.testNumber3, `Element at index ${i} should match`);
    }
}

function testReplaceAllElementsWithLambdaAndIndex() {
    let arrayList = new ArrayList<number>();
    initializeArrayList(arrayList, TestData.forNumber);
    arrayList.replaceAllElements((value: number, index: number): number => {
        return value + index;
    });
    for (let i: int = 0; i < arrayList.length; i++) {
        assertEQ(arrayList[i], TestData.forNumber[i] + i, `Element at index ${i} should match`);
    }
}

function testReplaceAllElementsWithLambdaIndexAndTempArrayList() {
    let arrayList = new ArrayList<number>();
    initializeArrayList(arrayList, TestData.forNumber);
    arrayList.replaceAllElements((value: number, index: number, tempArrayList: ArrayList<number>): number => {
        assertEQ(arrayList, tempArrayList, "TempArrayList should be this");
        return value + tempArrayList[index];
    });
    for (let i: int = 0; i < arrayList.length; i++) {
        assertEQ(arrayList[i], TestData.forNumber[i] * 2, `Element at index ${i} should match`);
    }
}

function testForEachNoParameterFunction() {
    let arrayList = new ArrayList<number>();
    initializeArrayList(arrayList, TestData.forNumber);
    let forEachCount: number = 0;
    arrayList.forEach(() => {
        forEachCount++;
    });
    assertEQ(forEachCount, TestData.testNumber5, "ForEach count should match arrayList length");
}

function testForEachFunctionWithValue() {
    let arrayList = new ArrayList<number>();
    initializeArrayList(arrayList, TestData.forNumber);
    let forEachCount: number = 0;
    let forEachFlag: number = 0;
    arrayList.forEach((value: number) => {
        if (value != arrayList[forEachCount++]) {
            forEachFlag = TestData.testNumber1;
        }
    });
    assertEQ(forEachFlag, 0, "ForEach should iterate and modify elements correctly");
    assertEQ(forEachCount, TestData.testNumber5, "ForEach count should match arrayList length");
}

function testForEachFunctionWithValueAndIndex() {
    let arrayList = new ArrayList<number>();
    initializeArrayList(arrayList, TestData.forNumber);
    let forEachCount: number = 0;
    let forEachFlag: number = 0;
    arrayList.forEach((value: number, index: number) => {
        if (value != arrayList[forEachCount] || index != forEachCount) {
            forEachFlag = TestData.testNumber1;
        }
        forEachCount++;
    });
    assertEQ(forEachFlag, 0, "ForEach should iterate and modify elements correctly");
    assertEQ(forEachCount, TestData.testNumber5, "ForEach count should match arrayList length");
}

function testForEachFunctionWithValueIndexAndTempArrayList() {
    let arrayList = new ArrayList<number>();
    initializeArrayList(arrayList, TestData.forNumber);
    let forEachCount: number = 0;
    let forEachFlag: number = 0;
    arrayList.forEach((value: number, index: number, tempArrayList: ArrayList<number>) => {
        assertEQ(arrayList, tempArrayList, "TempArrayList should be this");
        if (value != arrayList[forEachCount] || index != forEachCount ||
            tempArrayList[forEachCount] != arrayList[forEachCount]) {
            forEachFlag = TestData.testNumber1;
        }
        forEachCount++;
    });
    assertEQ(forEachFlag, 0, "ForEach should iterate and modify elements correctly");
    assertEQ(forEachCount, TestData.testNumber5, "ForEach count should match arrayList length");
}

function testForEachChangeElements() {
    let arrayList = new ArrayList<number>();
    initializeArrayList(arrayList, TestData.forNumber);

    arrayList.forEach((value: number) => {
        value = TestData.testNumber12;
    });
    for (let i: int = 0; i < arrayList.length; i++) {
        assertEQ(arrayList[i], TestData.forNumber[i], "ForEach should keep all elements the same");
    }
}

function initializeArrayList<T>(arrayList: ArrayList<T>, sourceArray: T[]) {
    for (let i = 0; i < sourceArray.length; i++) {
        arrayList.add(sourceArray[i]);
    }
}
