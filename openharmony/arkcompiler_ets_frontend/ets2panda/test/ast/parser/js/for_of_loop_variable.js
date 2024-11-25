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

const iterable = [10, 20, 30];

for (const value = 50 /* @@ label */of iterable) {
  console.log(value);
}


const iterable2 = [10, 20, 30];

for (let value of iterable2) {
  value += 50;
  console.log(value);
}

/* @@@ label Error SyntaxError: for-of loop variable declaration may not have an initializer */
