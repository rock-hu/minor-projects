/* @@ label2 */
/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

enum Color {
    Red = /* @@ label */4 / 3.0
}

enum Date {
    Day = /* @@ label1 */Date.now()
}

/* @@@ label Error TypeError: Enumeration members can be initialized only by compile-time expressions and initializers must be of the same type.  */
/* @@@ label1 Error TypeError: Enumeration members can be initialized only by compile-time expressions and initializers must be of the same type.  */
/* @@@ label2 Error TypeError: Class 'Date' is already defined with different type.  */
