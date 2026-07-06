/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { Formatter } from '../formatter/Formatter';
import { Level } from '../Level';
import { LoggerContext } from '../slf4j/LoggerContext';

export function makeMsg(loggerContext: LoggerContext, pattern: string| undefined, level: Level, msg: string) {
  let result = '';
  if (pattern) {
    result = Formatter.format(loggerContext, pattern, level.getLevelStr(), msg);
  } else {
    result = msg;
  }
  return result;
}