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
    static foo : number = 3
}

function f(): A { 
    return new A()
}

function main() {
  f()./* @@ label1 */foo
  f()!./* @@ label2 */foo
}

/* @@@ label1 Error TypeError: 'foo' is a static property of 'A' */
/* @@@ label2 Error TypeError: 'foo' is a static property of 'A' */
