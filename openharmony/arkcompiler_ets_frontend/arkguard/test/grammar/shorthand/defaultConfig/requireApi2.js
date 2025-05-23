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

import assert from 'assert';

function functionRequireTest(){
  const {
    codes2: { ERR_INVALID_ARG_TYPE_2, ERR_MISSING_ARGS_2, ERROR_OUT_OF_RANGE_2 },
    AbortError2,
  } = require('./exportsApi2');

  assert(ERR_INVALID_ARG_TYPE_2 === 1, 'success');
  assert(ERR_MISSING_ARGS_2 === 2, 'success');
  assert(ERROR_OUT_OF_RANGE_2 === 3, 'success');
  assert(AbortError2 === 4, 'success');
}

functionRequireTest();