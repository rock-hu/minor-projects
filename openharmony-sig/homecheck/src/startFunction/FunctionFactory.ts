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
import { BaseFunction } from "./BaseFunction";
import LoggerFactory from "../utils/LoggerFactory";

export class FunctionFactory {
    public static async createFunction(builder: string): Promise<BaseFunction | null> {
        const logger = LoggerFactory.getLogger('FunctionFactory');
        try {
            const builderFunction = (await import(`./FunctionFor${builder}`)).default;
            if (builderFunction && builderFunction.prototype instanceof BaseFunction) {
                logger.info(`Create function for ${builder}`);
                return new builderFunction();
            } else {
                logger.error(`Failed to create function for ${builder}. Imported function is not a subclass of BaseFunction.`);
                return null;
            }
        } catch (error) {
            logger.error(`Failed to load function for ${builder}`, error);
            return null;
        }
    }
}