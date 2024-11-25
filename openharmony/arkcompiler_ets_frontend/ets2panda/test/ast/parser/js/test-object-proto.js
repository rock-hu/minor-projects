/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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


var a = {
  get __proto__(){}, 
  [__proto__]: 5,
  __proto__: 7,
  __proto__,
  __proto__(){},
  __proto__(){},
  *__proto__(){},
  /* @@ label */__proto__: 8,
}
/* @@@ label Error SyntaxError: Duplicate __proto__ fields are not allowed in object literals  */
