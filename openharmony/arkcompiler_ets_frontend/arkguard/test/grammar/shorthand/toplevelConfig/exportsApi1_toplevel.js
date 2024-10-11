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

// 定义一些常量
const ERR_INVALID_ARG_TYPE1 = 'ERR_INVALID_ARG_TYPE';
const ERR_MISSING_ARGS1 = 'ERR_MISSING_ARGS';
const ERROR_OUT_OF_RANGE1 = 'ERROR_OUT_OF_RANGE';

// 定义一个异常类
class AbortError1 extends Error {
  constructor(message) {
    super(message);
    this.name = 'AbortError';
  }
}

// 导出常量和类
module.exports = {
  codes1: {
    ERR_INVALID_ARG_TYPE1,
    ERR_MISSING_ARGS1,
    ERROR_OUT_OF_RANGE1,
  },
  AbortError1,
};
