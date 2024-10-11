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

let arraySize: number = 3
let arr:number[] = [1,2,3]
let res:Float64Array = new Float64Array([0,0,0])
for(let i = 0; i < 10; i++) { // cannot access one memony
    res[i % arraySize] = arr[i % 3];
    res[i % arraySize] = arr[i % 3];
}

let arr2:Float64Array = new Float64Array([0,0,0])
for(let i = 0; i < 10; i++) { // may access one memony
    res[i % arraySize] = arr2[i % 3];
    res[i % arraySize] = arr2[i % 3];
}