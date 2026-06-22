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
import { FunctionFactory } from './startFunction/FunctionFactory';
import LoggerFactory from './utils/LoggerFactory';

let logger = LoggerFactory.getLogger('Index');

async function start() {
    logger.info('Check start.');
    const builder = process.env.BUILDER;
    if (!builder) {
        logger.error('BUILDER is not set in process env.');
        process.exit(-1);
    }
    logger.info(`Builder is: ${builder}`);
    (await FunctionFactory.createFunction(builder))?.start();
    logger.info('Check end.');
    return 0;
}

(async function main() {
    const startTime = new Date().getTime();
    const exitcode = await start();
    logger.info(`Cli completed, exitCode: ${exitcode}`);
    const endTime = new Date().getTime();
    logger.info(`Check took: ${(endTime - startTime) / 1000} s.`);
    process.exit(exitcode);
})();