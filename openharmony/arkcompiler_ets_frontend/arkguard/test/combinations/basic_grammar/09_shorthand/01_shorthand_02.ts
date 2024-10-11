/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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


import assert from 'assert';

let name1 = 1;
let obj = {name1};
{
  const {name1 = 2} = obj;
  name1;
  assert(name1 === 1);
}
assert(obj.name1 === 1);

(function() {
  var s0;
  for ({s0=5} of [{s0:1}]) {
    assert(s0 === 1);
  }
})();
(function() {
  var s1;
  for ({s1:s1=5} of [{s1}]) {
    assert(s1 === 5);
  }
})();
(function() {
  let y;
  ({y=5} = {y:1})
  assert(y === 1);
})();
(function() {
  let y2:string, y3: {x:number};
  let obj:any = {y2:"1",y3:{x:2}};
  ({y2:y2 = '5', y3:y3={x:1}}=obj);
  assert(y2 === '1');
  assert(y3.x === 2);
})();
(function() {
  let z;
  ({z:z={x:5}}={z:{x:1}});
  assert(z.x === 1)
})();
