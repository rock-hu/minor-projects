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

interface I {
    foo(): this;
}

abstract class A implements I {
    foo/* @@ label */(): A {
        return this;
    }
}

/* @@@ label Error TypeError: foo(): A in A cannot override foo(): I in I because overriding return type is not compatible with the other return type. */
/* @@@ label Error TypeError: Method foo(): A in A not overriding any method */
