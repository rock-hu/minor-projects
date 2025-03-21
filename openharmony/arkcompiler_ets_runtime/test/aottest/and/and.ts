/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

declare function assert_equal(a: Object, b: Object):void;
var x1 = 15;
var y1 = 1;
var r1 = x1 & y1;
assert_equal(r1, 1);

var x2 = 15.8;
var y2 = 1;
var r2 = x2 & y2;
assert_equal(r2, 1);

var x3 = 15;
var y3 = 1.8;
var r3 = x3 & y3;
assert_equal(r3, 1);

var x4 = 15.8;
var y4 = 1.8;
var r4 = x4 & y4;
assert_equal(r4, 1);

var x5:any = "15";
var y5:number = 1;
var r5 = x5 & y5;
assert_equal(r5, 1);

var x6 = -15;
var y6 = 1;
var r6 = x6 & y6;
assert_equal(r6, 1);

var x7 = 15;
var y7 = -1;
var r7 = x7 & y7;
assert_equal(r7, 15);

// not supported type: string, bigint

test_end();