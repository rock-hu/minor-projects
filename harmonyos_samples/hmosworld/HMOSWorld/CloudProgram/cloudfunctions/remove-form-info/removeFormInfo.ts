/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { DatabaseHelper } from './DatabaseHelper';

let myHandler = async function (event, _context, callback, logger) {
  const formId: string = event.body ? JSON.parse(event.body).formId : event.formId;
  const token: string = event.body ? JSON.parse(event.body).token : event.token;

  if (!formId || !token) {
    callback({
      code: 1,
      message: '[remove-form-info] params can not empty',
      data: null,
    });
  }

  try {
    const databaseHelper: DatabaseHelper = await new DatabaseHelper(logger);
    const result: number = await databaseHelper.removeFormInfo(formId, token);
    if (result > 0) {
      callback({
        code: 0,
        message: '[remove-form-info] operation successful',
        result: event.body
      });
    } else {
      callback({
        code: 1,
        message: '[remove-form-info] operation failed',
        result: event.body
      });
    }
  } catch (err) {
    logger.error(`[remove-form-info] func error: ${err.message}`);
    callback({
      code: 3,
      message: '[remove-form-info] operation exception',
      data: err,
    });
  }
};

export { myHandler };