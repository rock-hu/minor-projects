/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

class Outer {
    /* @@ label */class Base {
        private a(): void {};
    }

    /* @@ label1 */class Derived extends Base {
        foo(): void {
            let base: Base = new Base();
            base.a();
            this.a();
        }
    }
}

/* @@@ label Error SyntaxError: Unexpected token. A constructor, method, accessor, or property was expected.  */
/* @@@ label1 Error SyntaxError: Unexpected token. A constructor, method, accessor, or property was expected. */
/* @@? 24:13 Error TypeError: Signature a(): void is not visible here. */
/* @@? 24:13 Error TypeError: No matching call signature */
/* @@? 25:13 Error TypeError: Signature a(): void is not visible here. */
/* @@? 25:13 Error TypeError: No matching call signature */
