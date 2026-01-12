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
import { IssueReport } from "../object/IssueReport";
import { Argument } from "../object/Argument";
import LoggerFactory from "../utils/LoggerFactory";
import { IgnoreFile } from "../object/IgnoreFile";

export function afterHandleForCommunity(issueReports: IssueReport[], argument: Argument) {
    let logger = LoggerFactory.getLogger('AfterHandleForCommunity');
    let ignoreFiles = argument.ignoreFiles;
    if (!ignoreFiles) {
        return;
    }
    issueReports.forEach(issueReport => {
        if (!isIgnored(issueReport, ignoreFiles)) {
            logger.info(issueReport.defect.mergeKey);
        }
    });
}

function isIgnored(issueReport: IssueReport, ignoreFiles: IgnoreFile[]) {
    let filePath = issueReport.filePath;
    let ruleId = issueReport.defect.ruleId;
    let line = issueReport.defect.reportLine;
    ignoreFiles.forEach(ignoreFile => {
        let ignoreLines = ignoreFile.ignoreLines;
        if (ignoreFile.filePath === filePath) {
            ignoreLines.forEach(ignoreLine => {
                if (ignoreLine.lineNumber === line) {
                    let ignoreRuleIds = ignoreLine.ruleIds;
                    ignoreRuleIds.forEach(ignoreRuleId => {
                        if (ignoreRuleId === ruleId) {
                            return true;
                        }
                    });
                }
            });
        }
    });
    return false;
}