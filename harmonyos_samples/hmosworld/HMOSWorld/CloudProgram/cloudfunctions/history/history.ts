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
import { ResourceResp } from './model/ResourceResp';

let myHandler = async function (event, _context, callback, logger) {
  const userId: string = event.body ? JSON.parse(event.body).userId : event.userId;

  if (!userId) {
    callback({
      code: 1,
      message: '[history] userId can not empty',
      data: null,
    });
  }

  try {
    const databaseHelper: DatabaseHelper = new DatabaseHelper(logger);
    const result: ResourceResp[] = await databaseHelper.queryResource(userId);
    if (result) {
      callback({
        code: 0,
        message: '[history] request successful',
        data: result,
      });
    } else {
      callback({
        code: 1,
        message: '[history] request failed',
        data: null,
      });
    }
  } catch (err) {
    logger.error(`[history] func error: ${err.message}`);
    callback({
      code: 3,
      message: '[history] operation exception',
      data: err,
    });
  }
};

export { myHandler };