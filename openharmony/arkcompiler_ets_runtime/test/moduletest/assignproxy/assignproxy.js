/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
 * @tc.name:assignproxy
 * @tc.desc:test assign for proxy
 * @tc.type: FUNC
 * @tc.require: issueI5NO8G
 */
var log = "";
var proxy = new Proxy({"cc":"dd", "ee":"ff"}, {
get: function(target, propertyKey, receiver) {
log += "get";
return target[propertyKey];
},
});

Object.assign({1:2, "aa":"bb"}, proxy);
print(log === "getget")

var a = {};
var b = { bProp: 1 };
var date1 = new Date();
var aSymbol = Symbol("aSymbol");
b[aSymbol] = 2;
b[date1] = 5;
Object.assign(a, b);
print(a.bProp);
print(a[aSymbol]);
print(a[date1]);

var c = {};
var d = { bProp: 1, two: 2, tre: 3 };
d[aSymbol] = 6;
d[date1] = 10;
delete d.bProp;
Object.assign(c, d);
print(c.two);
print(c[aSymbol]);
print(d[date1]);