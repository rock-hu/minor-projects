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

function func(this: int[]) {}

/* @@? 16:1 Node {
  "type": "MethodDefinition",
  "kind": "extensionmethod",
  "accessibility": "public",
  "static": true
} */

/* @@? 16:10 Node {
  "type": "ETSPrimitiveType"
} */

/* @@? 16:13 Node {
  "type": "TSArrayType"
} */

/* @@? 16:16 Node {
  "type": "Identifier",
  "name": "func"
} */

/* @@? 16:21 Node {
  "type": "Identifier",
  "name": "=t"
} */

/* @@? 16:23 Node {
  "type": "BlockStatement",
  "statements": []
} */
