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

class A {
  static $_get(i : string) : number {
      return 42.0
  }
}

function main() {
    let a = new A()
    let b = a./* @@ label */$_get('a')
    let c = A.$_get('a')
    let d = a[/* @@ label1 */'a']
    let e = /* @@ label2 */A[/* @@ label3 */'a']
}

/* @@@ label Error TypeError: '$_get' is a static property of 'A' */
/* @@@ label1 Error TypeError: '$_get' is a static property of 'A' */
/* @@@ label2 Error TypeError: Class name 'A' used in the wrong context */
/* @@@ label3 Error TypeError: '$_get' is a static property of 'A' */
