/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

function varDeclaration() {
  var x = 20;

  function f(shouldInitialize: boolean) {
    if (shouldInitialize) {
      var y = 10;
    }
    return y;
  }
  f(true); // returns '10'
  f(false); // returns 'undefined'"

  var array = [1, 2, 3, 4];
  for (var i = 0; i < array.length; i++) console.log(array[i]);

  for (var j in array) {
    if (true) {
      console.log(array[j]);
    }
  }

  for (var e of array) console.log(e);
}
