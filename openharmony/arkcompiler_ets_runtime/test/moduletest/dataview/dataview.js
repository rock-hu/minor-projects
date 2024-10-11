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
 * @tc.name:dataview
 * @tc.desc:test dataview
 * @tc.type: FUNC
 * @tc.require: issue#I7NUZM
 */
const buffer = new ArrayBuffer(16);
const view = new DataView(buffer);
view.setInt32({}, 0x1337, {});
print(view.getInt32({}, {}));

try {
    var buffer1 = new ArrayBuffer(64);
    var dataview = new DataView(buffer1, 8, 24);
    dataview.setInt32(0, 1n);
} catch(e) {
    print(e)
}

const buf = new ArrayBuffer(16);
const first = new DataView(buf, 0, 8);
const second = new DataView(buf, 8);
// test setInt32
second.setInt32(0, NaN);
print(second.getInt32(0));
second.setInt32(0, 13.54);
print(second.getInt32(0));
second.setInt32(0, -413.54);
print(second.getInt32(0));
second.setInt32(1, 2147483648);
print(second.getInt32(1));
second.setInt32(1, 13.54);
print(second.getInt32(0));
second.setInt32(0, Infinity);
print(second.getInt32(0));
second.setInt32(0, 27, true);
print(second.getInt32(0));

// test setFloat32
second.setFloat32(0, NaN);
print(second.getInt32(0));
second.setFloat32(0, 13.54);
print(second.getInt32(0));
second.setFloat32(0, -413.54);
print(second.getInt32(0));
second.setFloat32(1, 2147483648);
print(second.getInt32(1));
second.setFloat32(1, 13.54);
print(second.getInt32(0));
second.setFloat32(0, Infinity);
print(second.getInt32(0));
second.setFloat32(0, 27, true);
print(second.getInt32(0));

// test setFloat64
second.setFloat64(0, NaN);
print(second.getInt32(0));
second.setFloat64(0, 13.54);
print(second.getInt32(0));
second.setFloat64(0, -413.54);
print(second.getInt32(0));
second.setFloat64(0, 2147483648);
print(second.getInt32(1));
second.setFloat64(0, 13.54);
print(second.getInt32(0));
second.setFloat64(0, Infinity);
print(second.getInt32(0));
second.setFloat64(0, 27, true);
print(second.getInt32(0));