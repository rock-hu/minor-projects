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

import { ff1 } from "./interop_import_js_rules_js"

export {ff1} // imported from js. Error is shown

export { ff2 } from "./interop_import_js_rules_js" // ff2 is imported from js. Error is shown

export { foo as bar } from "./oh_modules/reflect_export" // foo is imported from arkts1.2. No error.

export * as namespace2 from "./interop_import_js_rules_js"
