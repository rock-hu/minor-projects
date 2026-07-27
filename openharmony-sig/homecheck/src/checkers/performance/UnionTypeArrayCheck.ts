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
import { ArkAssignStmt, ArkClass, ArkFile, ArkMethod, ArkNewArrayExpr,
    ArrayType, Local, NumberType, Scene, Stmt, UnionType, Value} from "@ArkAnalyzer/src";
import { BaseChecker } from "../BaseChecker";
import { CheckerStorage } from "../../object/CheckerStorage";
import { Defects } from "../../object/Defects";
import { NumberUtils } from "src/utils/NumberUtils";
import { IssueReport } from "../../object/IssueReport";
import { Rule } from "../../object/Rule";
import { SceneUtils } from "../../utils/SceneUtils";
import LoggerFactory from "../../utils/LoggerFactory";
import { ValueType } from "src/object/NumberValue";
import { StmtExt } from "src/object/scope/StmtExt";
import { VarInfo } from "src/object/scope/VarInfo";

let logger = LoggerFactory.getLogger('UnionTypeArrayCheck');

export default class UnionTypeArrayCheck extends BaseChecker {
    private gSeverity = -1;
    private readonly ruleId = '@performance/union-type-array-check';
    private readonly ruleDocPath = 'docs/union-type-array-check.md';
    private readonly description = 'Suggestion: Avoid using arrays of union types.';
    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        logger.debug(`Start UnionTypeArrayCheck.`);
        this.gSeverity = rule ? rule.alert : 3;
        let issueReport: IssueReport[] = [];
        let arkFiles = SceneUtils.getArkFile(filePath,scene);

        for (let arkFile of arkFiles) {
            for (let clazz of arkFile.getClasses()) {
                this.unionClassProcess(clazz, issueReport);
            }
            for (let namespace of arkFile.getAllNamespacesUnderThisFile()) {
                for (let clazz of namespace.getClasses()) {
                    this.unionClassProcess(clazz, issueReport);
                }
            }
        }
        return issueReport;
    }

    public unionClassProcess(arkClass: ArkClass, issueReports: IssueReport[]): void {
        let arkMethods = arkClass.getMethods();
        for (let arkMethod of arkMethods) {
            this.methodProcess(arkMethod, arkClass, issueReports);
        }
    }

    public methodProcess(arkMethod: ArkMethod, clazz: ArkClass, issueReports: IssueReport[]): void {
        const stmts = arkMethod.getBody()?.getCfg()?.getStmts();
        if (!stmts) {
            return;
        }
        let arkFile = clazz.getDeclaringArkFile();
        for (let stmt of stmts) {
            if (!(stmt instanceof ArkAssignStmt)) {
                continue;
            }
            let rightOp = stmt.getRightOp();
            if (!rightOp || !(rightOp instanceof ArkNewArrayExpr)) {
                continue;
            }
            let leftOp = stmt.getLeftOp();
            let leftType = leftOp.getType();
            if (!(leftType instanceof ArrayType)) {
                continue;
            }
            let type = leftType.getBaseType();
            if (type instanceof UnionType) {
                let arrayName = this.getArrayName(leftOp);
                if (arrayName === undefined) {
                    continue;
                }
                this.reportIssue(arkFile, issueReports, stmt, arrayName);
            } else if (type instanceof NumberType) {
                if (!(this.numberCheck(leftOp, clazz))) {
                    continue;
                }
                let arrayName = this.getArrayName(leftOp);
                if (arrayName === undefined) {
                    continue;
                }
                this.reportIssue(arkFile, issueReports, stmt, arrayName);
            }
        }
    }

    public getArrayName(leftOp: Value): string | undefined {
        if (!(leftOp instanceof Local)) {
            return undefined;
        }
        let stmts = leftOp.getUsedStmts();
        if (!stmts) {
            return undefined;
        }
        for (let stmt of stmts) {
            if (!(stmt instanceof ArkAssignStmt)) {
                continue;
            }
            let rightOp = stmt.getRightOp();
            if (!(rightOp instanceof Local)) {
                continue;
            }
            if (!rightOp.getName().includes('%')) {
                continue;
            }
            let leftOp = stmt.getLeftOp();
            if (!(leftOp instanceof Local)) {
                return undefined;
            }
            return leftOp.getName();
        }
        return undefined;
    }

    public numberCheck(leftOp: Value, clazz: ArkClass): boolean {
        if (!(leftOp instanceof Local)) {
            return false;
        }
        let stmts = leftOp.getUsedStmts();
        if (!stmts) {
            return false;
        }
        let valueType: ValueType = ValueType.UNKNOWN;
        for (let [index, stmt] of stmts.entries()) {
            let scope = (stmt as StmtExt).scope;
            if (!scope || !(stmt instanceof ArkAssignStmt)) {
                continue;
            }
            let arkFile = clazz.getDeclaringArkFile();
            let rightOp = stmt.getRightOp();
            let varInfo: VarInfo = new VarInfo(stmt, scope);
            if (!NumberUtils.isValueSupportCalculation(arkFile, varInfo, rightOp)) {
                continue;
            }
            let reDefValue = NumberUtils.getNumberByScope(arkFile, varInfo, rightOp);
            if (index === 0) {
                valueType = reDefValue.type;
                continue;
            }
            if (reDefValue.type !== valueType) {
                return true;
            }
        }
        return false;
    }

    private reportIssue(arkFile: ArkFile, issueReports: IssueReport[], stmt: Stmt | undefined, keyword: string): void {
        if (!stmt) {
            return;
        }
        let originalPosition = stmt.getOriginPositionInfo();
        let lineNum = originalPosition.getLineNo();
        let orgStmtStr = stmt.getOriginalText();
        if (!orgStmtStr || orgStmtStr.length === 0) {
            return;
        }
        let startColumn = originalPosition.getColNo() + orgStmtStr.indexOf(keyword);
        let endColumn = startColumn + keyword.length - 1;
        if (startColumn === -1) {
            return;
        }
        let filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        const fixKey = lineNum + '%' + startColumn + '%' + endColumn + '%' + this.ruleId;
        const mergeKey = filePath + '%' + fixKey + '%' + this.description;
        let defects = new Defects(lineNum, startColumn, this.description, this.gSeverity,
            this.ruleId, mergeKey, this.ruleDocPath, true, false, false, fixKey);
        let issueReport = new IssueReport(filePath, defects);
        let result = issueReports.find(issue => issue.defect.mergeKey === mergeKey);
        if (!result) {
            issueReports.push(issueReport);
        }
    }
}