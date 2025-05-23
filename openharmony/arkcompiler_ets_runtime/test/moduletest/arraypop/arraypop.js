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
 * @tc.name:arrayprotochange
 * @tc.desc:test Array function when prototype changes
 * @tc.type: FUNC
 * @tc.require: issueI7O616
 */

(function () {
  var array = [,];
  function pop() {
    return array.pop();
  }
  array.__proto__.push(6);
  print(pop());
})();

/*
 * @tc.name:arraypush
 * @tc.desc:test Array function while SetCapacity
 * @tc.type: FUNC
 * @tc.require: issues:IAFERE
 */

function testSetCapacity() {
  let arr = new Array();
  for (let i = 0; i < 10000; i++) {
      arr.push(i);
  }
  print("Test completed");
}

testSetCapacity();