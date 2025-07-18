/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

function main() {
  let resolve: ((value: string) => void) | null = null;

  let p = new Promise<string>((_resolve: (value: string)=> void): void => {
    resolve = _resolve;
  });

  resolve!("abc");                         // no smart cast!

  let x: Number|String|undefined = "test1";
  let y: Number|String|undefined = "test2"
  let z: Number|String|undefined = 7

  let lam: () => void = () => {
     let y: Number|String|undefined = 2;   // hides outer declaration!
     x = z; 
     let tmp: number = y;                  // smart cast is used
     assertEQ(tmp, 2);
  };

  if (x instanceof string) {
    let tmp: string = x as string;         // no smart cast!
    assertEQ(tmp, "test1");
  }	

  lam();

  assertEQ(x, 7);

  if (y instanceof string) {
    let tmp: string = y;                   // smart cast is used
    assertEQ(tmp, "test2");
  }     

  let w: number = z;                       // smart cast is used
  assertEQ(w, 7)
}
