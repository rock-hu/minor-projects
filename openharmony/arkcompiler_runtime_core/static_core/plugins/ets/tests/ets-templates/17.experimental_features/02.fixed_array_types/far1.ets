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

{% for c in cases %}
/*---
desc: >-
    Testing FixedArray<T> with various types of elements, with casting it to
    Object and back to the original type. Expecting successful result.
---*/

function toObj(p: Object|null|undefined): Object {
    return p!
}

{{c.decl}}

function main() {
    {{c.use|indent}}
}
{% endfor %}
