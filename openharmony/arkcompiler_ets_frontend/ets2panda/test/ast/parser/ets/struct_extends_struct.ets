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

struct A {

}

struct B extends A {
    
}


/* @@? 16:1 Error TypeError: Structs are only used to define UI components, it should be translated at 'plugin after parser' phase. */
/* @@? 20:1 Error TypeError: Structs are only used to define UI components, it should be translated at 'plugin after parser' phase. */
/* @@? 20:18 Error TypeError: struct B is not extensible. */
