/*
* Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

class Base {
    foo( i: number ): number {
        return i;
    }
}

class Derived extends Base {
    override foo(i: number): Number {
        return i;
    }
}

/* @@? 23:17 Error TypeError: foo(i: double): Double in Derived cannot override foo(i: double): double in Base because overriding return type is not compatible with the other return type.  */
/* @@? 23:17 Error TypeError: Method foo(i: double): Double in Derived not overriding any method  */