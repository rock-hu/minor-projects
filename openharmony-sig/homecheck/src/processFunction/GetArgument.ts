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
import path from 'path';
import { Argument } from '../object/Argument';
import { ConfigUtils } from '../utils/ConfigUtils';
import LoggerFactory from '../utils/LoggerFactory';
import { ParseCliOptions } from '../utils/ParseCliOptions';

let logger = LoggerFactory.getLogger('createArgumentByConfig');
const DEFAULT_CONFIG_PATH = path.join(__filename, 'Config.json');

export function createArgumentByConfig(): Argument | null{
    const configPath = ParseCliOptions(process.argv).configPath ?? DEFAULT_CONFIG_PATH;
    logger.info('configPath:' + configPath);
    const config = ConfigUtils.getConfig(configPath);
    if (!config) {
        return null;
    }
    const argument = new Argument();
    argument.generateArgument(config);
    logger.debug("argument: " + JSON.stringify(argument));
    return argument;
}