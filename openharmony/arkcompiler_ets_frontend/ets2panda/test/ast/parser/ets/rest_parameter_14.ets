/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at 
* 
* http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable low or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

class A {}

function foo(/* @@ label1 */...j: A) {}

class B {
  moo(/* @@ label2 */...i: A) {}
}

/* @@@ label1 Error SyntaxError: Rest parameter should be either array or tuple type. */
/* @@@ label2 Error SyntaxError: Rest parameter should be either array or tuple type. */
