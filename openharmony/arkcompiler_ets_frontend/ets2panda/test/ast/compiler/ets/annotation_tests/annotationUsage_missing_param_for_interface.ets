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

@interface MyAnno {
    testProperty1: string
    testProperty2: string
}

@MyAnno({testProperty2: ""})
interface itf {
    @MyAnno({testProperty1:""})
    foo(): void
    @MyAnno({testProperty1:""})
    x : int
}

/* @@? 21:2 Error TypeError: The required field 'testProperty1' must be specified. Fields without default values cannot be omitted. */
/* @@? 23:6 Error TypeError: The required field 'testProperty2' must be specified. Fields without default values cannot be omitted. */
/* @@? 25:6 Error TypeError: The required field 'testProperty2' must be specified. Fields without default values cannot be omitted. */
