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

/*
 * @tc.name:moduleUseCjs
 * @tc.desc:test module CJS
 * @tc.type: FUNC
 * @tc.require: issueI5NO8G
 */
import cjs from "./Cjs"
import {foo, bar} from "./moduleExportCjs.js"
import * as ns from "./someArgsCjs"
import {json, fun} from "./someArgsCjs"
import "./cjsWithoutExports.js"
import {SUCCESS} from "./CjsDictionaryMode"

print(JSON.stringify(cjs));
print(JSON.stringify(foo));
print(typeof bar);
print(JSON.stringify(json));
fun();
print(ns.tag);
ns.con();
print(SUCCESS);