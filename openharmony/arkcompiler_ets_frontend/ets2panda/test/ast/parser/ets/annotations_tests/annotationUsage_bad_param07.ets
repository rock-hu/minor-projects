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

@Retention("SOURCE")
@interface MyAnno {
    testProperty1: string
}


function foo(@MyAnno ...a){}

/* @@? 22:15 Error TypeError: The required field 'testProperty1' must be specified. Fields without default values cannot be omitted. */
/* @@? 22:22 Error SyntaxError: Rest parameter should be either array or tuple type. */
/* @@? 22:26 Error SyntaxError: Parameter declaration should have an explicit type annotation. */