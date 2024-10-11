/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @param para1
 * @param para2
 * @returns
 */
function testFunction(para1: number, para2: string) {
  if (typeof(para1) === 'string') {
    return 'Para1 is string';
  }
  // This is a single line comments
  if (typeof(para2) === 'number') {
    return 'Para2 is number';
  }
}