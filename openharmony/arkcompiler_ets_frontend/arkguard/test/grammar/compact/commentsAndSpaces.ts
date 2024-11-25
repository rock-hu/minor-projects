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

function foo1() {
  let a = 1;          // 验证空格的压缩情况 


          let b = 2;


  let c = 3;
  // 验证单行注释的压缩情况
  // commentssssss
  // commentssssss
  let d = 4;
  return a + b + c + d;
}

/*  
* 验证多行注释的压缩情况
* commentssssss
* commentssssss
*/ 
function foo2() {}