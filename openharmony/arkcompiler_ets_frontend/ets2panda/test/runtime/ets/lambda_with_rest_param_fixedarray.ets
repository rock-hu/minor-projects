/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

let foa:(c:string, ...args:FixedArray<number>)=>string = (c:string, ...args:FixedArray<number>)=>{
    return "c"
}

let fob:(...args:FixedArray<number>)=>number = (...args:FixedArray<number>):number =>{
    let sum = 0
    for(let e of args){
        sum += e
    }
    return sum
}


function main(){
    assertEQ(foa("1"), "c")
    assertEQ(fob(1, 2, 3, 4), 10)
}