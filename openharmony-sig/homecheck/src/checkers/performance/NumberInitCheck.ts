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
import { ArkAssignStmt, Scene, Stmt, Value } from "@ArkAnalyzer/src";
import { IssueReport } from "../../object/IssueReport";
import { Rule } from "../../object/Rule";
import { BaseChecker } from "../BaseChecker";
import { SceneUtils } from "../../utils/SceneUtils";
import { CheckerStorage } from "../../object/CheckerStorage";
import { Scope } from "src/object/scope/Scope";
import { ValueType } from "src/object/NumberValue";
import { VarInfo } from "src/object/scope/VarInfo";
import { NumberUtils } from "../../utils/NumberUtils";
import { Variable } from "src/object/scope/Variable";
import { ScopeUtils } from "src/utils/Scopeutils";
import { ScopeType } from "src/object/scope/ScopeType";
import { Defects } from "src/object/Defects";

export default class NumberInitCheck extends BaseChecker {
    private readonly description = 'Number variable of both int and float types detected. The value assigned to be a variable should be of the type declared for the variable.';
    private readonly ruleDocPath = 'docs/number-init-check.md';
    private readonly ruleId = '@performance/number-init-check';
    private numberInitSeverity: number = -1;
    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        this.numberInitSeverity = rule ? rule.alert : 1;
        let issueReports: IssueReport[] = [];
        let arkFiles = SceneUtils.getArkFile(filePath, scene);
        for (let f1 of arkFiles) {
            let filePath = f1.getFilePath();
            let scope = CheckerStorage.getInstance().getScope(filePath);
            if (scope) {
                this.traverseScope(scope, issueReports);
            }
        }
        return issueReports;
    }


    private traverseScope(scope: Scope, issueReports: IssueReport[]): void {
        this.parameteCheck(scope, issueReports);
        if (scope.childScopeList.length !== 0) {
            for(let childScope of scope.childScopeList) {
                this.traverseScope(childScope, issueReports);
            }
        }
    }

    private parameteCheck(scope: Scope, issueReports: IssueReport[]): void {
        if (scope.defList.length === 0) {
            return;
        }
        for (let defValueInfo of scope.defList) {
            let defType: ValueType = ValueType.UNKNOWN;
            let defStmt = defValueInfo.defStmt;
            let defStmtInfo = new VarInfo(defStmt, scope);
            if (defStmt instanceof ArkAssignStmt) {
                let rightOp = defStmt.getRightOp();
                defType = this.checkValueType(defStmtInfo, rightOp);
                if (defType === ValueType.UNKNOWN) {
                    continue;
                }
            }
            this.checkByDefValueInfo(defValueInfo, defType, issueReports);
        }
    }

    private checkValueType(varInfo: VarInfo, value: Value): ValueType {
        const arkFile = varInfo.stmt.getCfg()?.getDeclaringMethod().getDeclaringArkFile();
        if (arkFile) {
            if (!NumberUtils.isValueSupportCalculation(arkFile, varInfo, value)) {
                return ValueType.UNKNOWN;
            }
            let reDefValue = NumberUtils.getNumberByScope(arkFile, varInfo, value);
            return reDefValue.type;
        }
        return ValueType.UNKNOWN;
    }

    private checkByDefValueInfo(defValueInfo: Variable, defType: ValueType, issueReports: IssueReport[]): void {
        let reDefStmtInfos = defValueInfo.redefInfo;
        for (let reDefStmtInfo of reDefStmtInfos) {
            let reDefStmt = reDefStmtInfo.stmt;
            if (reDefStmt instanceof ArkAssignStmt) {
                let rightOp = reDefStmt.getRightOp();
                let reDefType = this.checkValueType(reDefStmtInfo, rightOp);
                if (reDefType === ValueType.UNKNOWN) {
                    break;
                } else if (reDefType != defType) {
                    this.setIssueReports(reDefStmt, issueReports);
                }
            }
        }
    }

    private setIssueReports(reDefStmt: Stmt, issueReports: IssueReport[]): void {
        const arkFile = reDefStmt.getCfg()?.getDeclaringMethod().getDeclaringArkFile();
        let originalPosition = reDefStmt.getOriginPositionInfo();
        const lineNo = originalPosition.getLineNo();
        const spacesColumn = originalPosition.getColNo();
        let text = reDefStmt.getOriginalText();
        if (!arkFile || !text || text.length === 0) {
            return;
        }
        const filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        const texts = text.split('\n');
        text = texts[0];
        let checkText = '';
        if (text.includes(';')) {
            if (ScopeUtils.getScopeType(reDefStmt) === ScopeType.FOR_CONDITION_TYPE) {
                checkText = text.substring(text.lastIndexOf(';') + 2, text.indexOf(';'));
                checkText = checkText.substring(checkText.indexOf('=') + 2);
            } else {
                checkText = text.substring(text.indexOf('=') + 2, text.indexOf(';'));
            }
        } else {
            checkText = text.substring(text.indexOf('=') + 2);
        }
        const checkTextLen = checkText.length;
        const startColumn = spacesColumn + text.indexOf(checkText);
        const endColumn = startColumn + checkTextLen - 1;
        let fixKey = lineNo + '%' + startColumn + '%' + endColumn + '%' + this.ruleId;
        let mergeKey = filePath + '%' + fixKey + '%' + this.description;
        issueReports.push(new IssueReport(filePath, new Defects(lineNo, startColumn, this.description, this.numberInitSeverity, 
            this.ruleId, mergeKey, this.ruleDocPath, true, false, false, fixKey)));
    }
}