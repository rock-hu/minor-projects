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

class C {
  index = 0
}

function main(): int {
  let c = new C()
  let res = ''
  for (let x of c) {
      res += x
  }
  if (res != '6') return 1
  return 0
}

/* @@? 23:17 Error TypeError: Object type doesn't have proper iterator method. */
/* @@? 23:17 Error TypeError: 'For-of' statement source expression is not of iterable type. */
