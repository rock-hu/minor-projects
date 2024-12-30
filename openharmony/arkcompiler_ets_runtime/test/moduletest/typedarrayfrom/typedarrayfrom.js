/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 * @tc.name:typedarrayfrom
 * @tc.desc:test TypedArray.from
 * @tc.type: FUNC
 */

let int8Arr = new Int8Array();
let it = int8Arr[Symbol.iterator]();
it.__proto__["next"] = function() {
    return { value:undefined, done:true };
}
let newInt8Arr = Int8Array.from(int8Arr);
assert_equal(newInt8Arr.length,0);

try {
  let int8Arr = new Int8Array();
  let it = int8Arr[Symbol.iterator]();
  it.__proto__["next"] = new Map()[Symbol.iterator]().__proto__["next"];
  let newInt8Arr = Int8Array.from(int8Arr);
  assert_unreachable();
} catch (e) {
    assert_equal(e instanceof TypeError, true);
}

int8Arr = new Int8Array();
int8Arr.__proto__.__proto__[Symbol.iterator] = function* () {
    yield 1;
    yield 2;
}
newInt8Arr = Int8Array.from(int8Arr);
assert_equal(newInt8Arr.length,2);

int8Arr.__proto__.__proto__[Symbol.iterator] = function() {
  return {
    next: function() {
      return { value:undefined, done:true };
    }
}
};
it = int8Arr[Symbol.iterator]();
newInt8Arr = Int8Array.from(int8Arr);
assert_equal(newInt8Arr.length,0);

let arr = new Array(10);
it = arr[Symbol.iterator]();
it.__proto__["next"] = function() {
  return { value:undefined, done:true };
}
let newArr = Int8Array.from(arr);
assert_equal(newArr.length,0);

const v1 = ([-4.0,415.6053436378277,0.0,-33773.81284924084,-5.0]).__proto__;
v1[Symbol.iterator] = 1;
function f2() {
    return f2;
}
class C3 extends f2 {
}
class C4 extends f2 {
    constructor(...args) {
        super(...args);
    }
}
try {
    new C3()  // since ES2021, default ctor of derivative class does not call Array.prototype[Symbol.iterator]
    new C4()  // spread syntax still calls Array.prototype[Symbol.iterator]
    assert_unreachable();
} catch (e) {
    assert_equal(e instanceof TypeError, true);
}

test_end();