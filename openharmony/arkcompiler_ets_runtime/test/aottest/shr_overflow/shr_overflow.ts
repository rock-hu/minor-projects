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

declare var ArkTools:any;


function test(){
  for(var i = 0; i<1000; i++){
    -1 >>> 0;
    -0;
    -(-2147483648);
  }
}
for(var i = 0;i<10000;i++) {
  test();
}

print(ArkTools.isAOTCompiled(test));
print(ArkTools.isAOTDeoptimized(test));