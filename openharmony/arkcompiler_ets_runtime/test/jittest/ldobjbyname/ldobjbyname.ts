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

class Rectangle {
    constructor(height, width) {
        this.height = height;
        this.width = width;
  }
}

function LoadValueByName(square) {
    return square.height;
}

const square = new Rectangle(10, 10);

for (let i = 0; i < 3; i++) {
    LoadValueByName(square)
}

ArkTools.jitCompileAsync(LoadValueByName);
let res = ArkTools.waitJitCompileFinish(LoadValueByName);
print(res);
try {
    print(LoadValueByName(0))
} catch(err) {
    print("catch")
}
