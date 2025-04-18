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
 * @tc.name:weakrefforsymbol
 * @tc.desc:test WeakRef allow the use of most Symbols as targets
 * @tc.type: FUNC
 * @tc.require: issueI7J2VN
 */

let target1 = Symbol("symbol1");
let wr1 = new WeakRef(target1);

let target2 = Symbol.for("symbol2");
try {
    let wr2 = new WeakRef(target2);
} catch (err) {
    assert_equal(err instanceof TypeError, true);
}

const symbolFuncsRef = [
    Symbol.asyncIterator,
    Symbol.hasInstance,
    Symbol.isConcatSpreadable,
    Symbol.iterator,
    Symbol.match,
    Symbol.matchAll,
    Symbol.replace,
    Symbol.search,
    Symbol.species,
    Symbol.split,
    Symbol.toPrimitive,
    Symbol.toStringTag,
    Symbol.unscopables
];

const symbolFuncsRefAssertLsit = [
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true
];
let symbolFuncsRefResLsit = [];

symbolFuncsRef.forEach(function (ctor, i) {
    let wr = new WeakRef(ctor);
    symbolFuncsRefResLsit.push(wr.deref() == ctor);
});

assert_equal(symbolFuncsRefResLsit, symbolFuncsRefAssertLsit);

test_end();