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
'use static'

const arr: int[] = [1, 2, 3, 4];

for (let i = 0; i < arr.length; i++) {
    arr[i]; //legal
}

for (let i = 0; i < 100; i++) {
    console.log(i); //legal
}

const arr2: int[] = [1, 2, 3, 4];
for (let i = 0; i < 100; i++) {
    arr2[10] //should report
}

const arr3: int[] = [1, 2, 3, 4];
for (let i = 0; i < arr3.length; i++) {
    arr3[10] //should report
}

const arr4: int[] = [1, 2, 3, 4];
let x: int = 3;
for (let i = 0; i < arr4.length; i++) {
    arr4[x]; //should report
}

const arr5: int[] = [1, 2, 3, 4];
for (let i = 0; i < 10; i++) {
    arr5[i]; //should report
}


const arr6: int[] = [1, 2, 3, 4];
if (arr6.length > 10) {
    arr6[10]
}

const arr7: int[] = [1, 2, 3, 4];
if (arr7.length > 10) {
    return;
}

arr7[10]

const arr8: int[] = [1, 2, 3, 4];
const index: int = 9;
if (arr8.length > 10 && index > 0) {
    return;
}

arr8[index];

const arr9: int[] = [1, 2, 3, 4];
if (arr9.length > 10 && index > 0) {
    arr9[index];
}

const arr10: int[] = [1, 2, 3, 4];
if (index > 0) {
    arr10[index];
}

const arr10: int[] = [1, 2, 3, 4];
let newIndex = 10;
if (arr10.length > newIndex) {
    return;
}

newIndex = 22;

arr10[newIndex];

let arr = [0, 1, 2, 3, 4, 5]
for(let i = 0; i < arr.length; i++) {
arr[i] = arr[i] + 1;
}
for(let i = 0; i < arr.length; i++) {
i = 10;
arr[i] = arr[i] + 1;
}

let arr = [0, 1, 2, 3, 4, 5]
let idx = 2;
if(idx > 0 && idx < arr.length) {
arr[idx] = arr[idx] + 1;
}
if(idx > 0 && idx < arr.length) {
idx = 10;
arr[idx] = arr[idx] + 1;
}

let arr = [0, 1, 2, 3, 4, 5]
let idx = 0;
while(idx > 0 && idx < arr.length) {
arr[idx] = arr[idx] + 1;
idx++;
idx = 10;
}
while(idx > 0 && idx < arr.length) {
idx = 10;
arr[idx] = arr[idx] + 1;
}

let arr = [0, 1, 2, 3, 4, 5]
let idx = 0;
arr[idx];
arr[10];
if (arr.length > 10) {
arr[10] = 10;
}

function foo():int{
return 1;
}

arr[44/3];
arr[foo()];
arr[()=>{return 1}];
if(arr.length > foo()) {
arr[foo()];
}
if(arr.length > 44/3) {
arr[4*4/3];
}

let arr1:number[] = [1, 1.5,45,2]

function foo(i:number):number{
  return i;
}

arr1[3*5] = 23;
arr1[parseInt("16")] = 23;
arr1[foo(16)] = 23

let arr1:number[] = [1, 1.5,45,2]

arr1[Number.MAX_VALUE] = 23;
arr1[Number.MAX_SAFE_INTEGER] = 23;

let arr1:number[] = [1, 1.5,45,2]
function foo(i:number):number{
  return i;
}
arr1[(24)] = 23;
arr1[+24] = 23;
enum TE{
  AA = 12
}
arr1[TE.AA] = 12;

let a: string[] = [];
let b: Array<string> = new Array(a.length);
for (let i = 0; i < a.length; i++) {
  b[i];
}