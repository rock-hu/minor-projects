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

let exportVar = 1;

exports.myExportVar = exportVar;
exports.myVariable = 2;
exports.myFunction = function() {
  return 'Hello, world!';
};

class CommonjsClass1 {
  id3 = '00001';
  info3 = {name3: 'jack3', age3: 12};
}

exports.CommonjsClass1 = CommonjsClass1;

exports.noNameClass = class {
  stuID = 11
  stuAge = 15
  getAge(){
    return this.stuAge;
  }
}