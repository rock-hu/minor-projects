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

class OtherType{}

@interface MyAnno {
    /* @@ label */testProperty1: OtherType;
    /* @@ label1 */testProperty3: [int ,number, string]
}

/* @@@ label Error TypeError: Invalid annotation field type. Only numeric, boolean, string, enum, or arrays of these types are permitted for annotation fields.  */
/* @@@ label1 Error TypeError: Invalid annotation field type. Only numeric, boolean, string, enum, or arrays of these types are permitted for annotation fields.  */