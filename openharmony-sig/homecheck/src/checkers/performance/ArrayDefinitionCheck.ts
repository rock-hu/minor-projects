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
import { ArkAssignStmt, ArkFile, ArkMethod, ArkNewArrayExpr, Scene, Stmt } from "@ArkAnalyzer/src";
import { IssueReport } from "../../object/IssueReport";
import { Rule } from "../../object/Rule";
import { BaseChecker } from "../BaseChecker";
import { SceneUtils } from "../../utils/SceneUtils";
import { CheckerStorage } from "../../object/CheckerStorage";
import { Defects } from "../../object/Defects";

export default class ArrayDefinitionCheck extends BaseChecker {
    private readonly descriptionArrayType = 'Array type definition is not correct';
    private readonly ARRAY_NAME = 'Array';
    private readonly ruleId = '@performance/array-definition-check';
    private readonly ruleDocPath = 'docs/array-definition-check.md';
    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        let severity = rule ? rule.alert : 3;
        let issueReport: IssueReport[] = [];
        let arkFiles = SceneUtils.getArkFile(filePath, scene);

        for (let arkFile of arkFiles) {
            for (let clazz of arkFile.getClasses()) {
                for (let method of clazz.getMethods()) {
                    this.processArkMethod(arkFile, method, issueReport, severity)
                }
            }
            for (let namespace of arkFile.getAllNamespacesUnderThisFile()) {
                for (let clazz of namespace.getClasses()) {
                    for (let method of clazz.getMethods()) {
                        this.processArkMethod(arkFile, method, issueReport, severity)
                    }
                }
            }
        }
        return issueReport;
    }

    public processArkMethod(arkFile: ArkFile, method: ArkMethod, issueReport: IssueReport[], severity: number) {
        for (let stmt of method.getBody()?.getCfg()?.getStmts() ?? []) {
            if (!(stmt instanceof ArkAssignStmt)) {
                continue;
            }
            let rightOp = stmt.getRightOp();
            if (!rightOp || !(rightOp instanceof ArkNewArrayExpr)) {
                continue;
            }
            let text = stmt.getOriginalText()??'';
            if (this.isGenericArray(text)) {
                this.reportIssue(arkFile, issueReport, stmt, text, severity);
            }
        }
    }

    private reportIssue(arkFile: ArkFile, issueReports: IssueReport[], stmt: Stmt, text: string, severity: number) {
        let filePath = arkFile.getFilePath();
        let lineNum = stmt.getOriginPositionInfo().getLineNo();
        let startColum = stmt.getOriginPositionInfo().getColNo() + text.indexOf(this.ARRAY_NAME);
        let endColum = stmt.getOriginPositionInfo().getColNo() + text.indexOf('=') - 1;
        filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        const fixKey = lineNum + '%' + startColum + '%' + endColum + '%' + this.ruleId
        const mergeKey = filePath + '%' + fixKey + '%' + this.descriptionArrayType
        let defects = new Defects(lineNum, startColum, this.descriptionArrayType, severity, this.ruleId, mergeKey,
            this.ruleDocPath, true, false, false, fixKey);
        let issueReport = new IssueReport(filePath, defects);
        issueReports.push(issueReport);
    }

    public isGenericArray(text: string) {
        let matchArr = text.match(/:([\s\S]*?)=/);
        if (matchArr !== null) {
            let matchTags = matchArr[1].match(/Array|<|>/g);
            return matchTags !== null && matchTags.includes('Array') && matchTags.includes('<') && matchTags.includes('>');
        }
        return false;
    }
}