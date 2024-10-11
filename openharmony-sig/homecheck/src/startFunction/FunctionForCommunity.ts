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
import { createArgumentByConfig } from "../processFunction/GetArgument";
import { buildSceneForCommunity } from "../processFunction/BuildScene";
import { createFileRulesMapForCommunity } from "../processFunction/GetFileRulesMap";
import { beforeHandleForCommunity } from "../processFunction/BeforeHandleForCommunity";
import { afterHandleForCommunity } from "../processFunction/AfterHandleForCommunity";
import { BaseFunction } from "./BaseFunction";
import { checkForCommunity } from "../processFunction/CheckBegin";
import LoggerFactory from "../utils/LoggerFactory";
//import {autoFix} from "../common/AutoFix";

let logger = LoggerFactory.getLogger('FunctionForCommunity');

export default class FunctionForCommunity extends BaseFunction {
    start() {
        const argument = createArgumentByConfig();
        if (!argument) {
            return;
        }
        logger.info("createArgumentByConfig completed.");

        const scene = buildSceneForCommunity(argument);
        logger.info("buildSceneForCommunity completed.");

        const fileRulesMap = createFileRulesMapForCommunity(argument);
        logger.info("createFileRulesMapForCommunity completed.");

        beforeHandleForCommunity(fileRulesMap, scene, argument);
        logger.info("beforeHandleForCommunity completed.");

        const issueReports = checkForCommunity(fileRulesMap, scene);
        logger.info("checkForCommunity completed.");

        afterHandleForCommunity(issueReports, argument);
        logger.info("afterHandleForCommunity completed.");

        // autoFix(issueReports[0], scene)
    }
}

