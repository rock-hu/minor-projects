/*
 * Copyright (c) 2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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
 * @tc.name:typedarraytosorted
 * @tc.desc:test TypedArray.toSorted
 * @tc.type: FUNC
 * @tc.require: issueI7MUIL
 */
const typedArraySortedConstructors = [
    Float64Array, Float32Array, Int32Array, Int16Array, Int8Array, Uint32Array, Uint16Array, Uint8Array,
    Uint8ClampedArray
];

const typedArraySortedConstructorsAssertSuccessList = [
	"Float64Array",
	"Float32Array",
	"Int32Array",
	"Int16Array",
	"Int8Array",
	"Uint32Array",
	"Uint16Array",
	"Uint8Array",
	"Uint8ClampedArray"
	];
const typedArraySortedConstructorsAssertFailList = [];
let typedArraySortedConstructorsSuccessList = [];
let typedArraySortedConstructorsFailList = [];

typedArraySortedConstructors.forEach(function (ctor, i) {
    let arr = [10, 3, 8, 5, 30, 100, 6, 7, 100, 3];
    if (testTypeArrayToSorted(ctor, arr)) {
        typedArraySortedConstructorsSuccessList.push(ctor.name);
    } else {
        typedArraySortedConstructorsFailList.push(ctor.name);
    }
});

assert_equal(typedArraySortedConstructorsSuccessList, typedArraySortedConstructorsAssertSuccessList);
assert_equal(typedArraySortedConstructorsFailList, typedArraySortedConstructorsAssertFailList);

const bigArrayAssertSuccessList = [
	"BigInt64Array",
	"BigUint64Array"
	];
const bigArrayAssertFailList = [];
let bigArraySuccessList = [];
let bigArrayFailList = [];

[
    BigInt64Array,
    BigUint64Array
].forEach(function (ctor, i) {
    let arr = [10n, 3n, 8n, 5n, 30n, 100n, 6n, 7n, 100n, 3n];
    if (testTypeArrayToSorted(ctor, arr)) {
        bigArraySuccessList.push(ctor.name);
    } else {
        bigArrayFailList.push(ctor.name);
    }
});

assert_equal(bigArraySuccessList, bigArrayAssertSuccessList);
assert_equal(bigArrayFailList, bigArrayAssertFailList);

function testTypeArrayToSorted(ctor, arr) {
    let obj = new ctor(arr);
    let arr2 = obj.toSorted();
    let result = [];
    result.push(arr2.toString() == "3,3,5,6,7,8,10,30,100,100");
    arr2 = obj.toSorted((x, y) => { return x < y });
    result.push(arr2.toString() == "100,100,30,10,8,7,6,5,3,3");
    try {
        arr2 = obj.toSorted(true);
    } catch (err) {
        result.push(err.name == "TypeError");
    }
    for (let i = 0; i < result.length; i++) {
        if (!result[i]) {
            return false;
        }
    }
    return true;
}

var arr = new Float64Array(["aaa", 10, 20, 30]);
var arrSorted = arr.toSorted();
assert_equal(arrSorted.toString(),"10,20,30,NaN");
assert_equal(arr.toString(),"NaN,10,20,30");

const arr1 = new BigUint64Array();
const arr2 = arr1.toReversed();
const arr3 = new Float64Array();
arr3.__proto__ = arr2;
const arr4 = arr3.toSorted(); // expect no crash
assert_equal(arr4.constructor.name, "Float64Array");
assert_equal(arr4.length, 0);

let arr3000flag = true;
function foo(){
    try {
        foo();
    } catch (err) {
    }
    if (arr3000flag) {
        const arr3000 = new Uint32Array(3000);
        const arr3000sort = arr3000.toSorted();  // expect no crash
        assert_equal(arr3000sort.constructor.name, "Uint32Array");
        assert_equal(arr3000sort.length, 3000);
        arr3000flag = false;
    }
}
foo();

test_end();