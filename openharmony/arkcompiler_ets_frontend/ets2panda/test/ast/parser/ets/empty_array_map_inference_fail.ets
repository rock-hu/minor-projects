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

(function() {
  return 37;
})();

[].map(function(x) {
  return x+1; 
});

/* @@? 16:2 Error SyntaxError: Function expressions are not supported, use arrow functions instead */
/* @@? 20:1 Error TypeError: Can't resolve array type */
/* @@? 20:8 Error SyntaxError: Function expressions are not supported, use arrow functions instead */
/* @@? 20:18 Error SyntaxError: Parameter declaration should have an explicit type annotation. */


