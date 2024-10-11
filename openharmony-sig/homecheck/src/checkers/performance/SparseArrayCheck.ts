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
import {
    ArkArrayRef,
    ArkAssignStmt,
    ArkFile,
    ArkNewArrayExpr,
    ArrayType,
    Local,
    NumberType,
    Scene,
    Stmt,
    Value
} from "@ArkAnalyzer/src";
import { IssueReport } from "src/object/IssueReport";
import { Rule } from "src/object/Rule";
import { BaseChecker } from "../BaseChecker";
import LoggerFactory from "../../utils/LoggerFactory";
import { SceneUtils } from "../../utils/SceneUtils";
import { CheckerStorage } from "../../object/CheckerStorage";
import { Scope } from "../../object/scope/Scope";
import { VarInfo } from "../../object/scope/VarInfo";
import { NumberUtils } from "../../utils/NumberUtils";
import { Defects } from "../../object/Defects";
import { SparseArrayType, SparseArrayValue } from "../../object/SparseArrayValue";

const logger = LoggerFactory.getLogger("SparseArrayCheck");
export default class SparseArrayCheck extends BaseChecker {
    private description = "Sparse array detected. Avoid using sparse arrays.";
    private ruleId = "@performance/sparse-array-check";
    private ruleDocPath = "docs/sparse-array-check.md";
    private issueColumnInTs = new Map<string, Array<string>>();


    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {

        let severity = rule ? rule.alert : 1;
        let issueReports: IssueReport[] = [];
        let arkFiles = SceneUtils.getArkFile(filePath, scene);
        for (let arkFile of arkFiles) {
            let arkFilePath = arkFile.getFilePath();
            this.issueColumnInTs.set(arkFilePath, new Array<string>());
            let parentScope = CheckerStorage.getInstance().getScope(arkFilePath);
            if (!parentScope) {
                continue;
            }
            //获取所有scope
            let scopes: Scope[] = [];
            this.traverseScope(parentScope, scopes);
            for (let scope of scopes) {
                this.findSparseArrayInScope(arkFile, scope, issueReports, severity);
            }
        }
        this.issueColumnInTs.clear();
        return issueReports;
    }

    private findSparseArrayInScope(arkFile: ArkFile, scope: Scope, issueReports: IssueReport[], severity: number) {
        for (let varDef of scope.defList) {
            for (let leftUsedVarInfo of varDef.leftUsedInfo) {
                let useStmt = leftUsedVarInfo.stmt;
                let def = useStmt.getDef();
                if (!def || !(def instanceof ArkArrayRef)) {
                    continue
                }
                let pIndex = def.getIndex();
                this.valueCalculate(arkFile, issueReports, useStmt, leftUsedVarInfo, pIndex, severity)
            }
            let defStmt = varDef.defStmt;
            if (!(defStmt instanceof ArkAssignStmt)) {
                continue;
            }
            let leftOp = defStmt.getLeftOp();
            let rightOp = defStmt.getRightOp();
            let isArray = (leftOp !== null) && (leftOp.getType() instanceof ArrayType);
            if (isArray && rightOp instanceof ArkNewArrayExpr) {
                let size = rightOp.getSize();
                let varInfo = new VarInfo(defStmt, scope);
                this.valueCalculate(arkFile, issueReports, defStmt, varInfo, size, severity);
            }

            if (rightOp instanceof ArkArrayRef) {
                let pIndex = rightOp.getIndex();
                let reDefInfo = new VarInfo(defStmt, scope);
                this.valueCalculate(arkFile, issueReports, defStmt, reDefInfo, pIndex, severity);
            }
        }
    }

    private valueCalculate(arkFile: ArkFile, issueReports: IssueReport[], stmt: Stmt, varInfo: VarInfo, value: Value, severity: number) {
        if (NumberUtils.isValueSupportCalculation(arkFile, varInfo, value)) {
            let index = NumberUtils.getNumberByScope(arkFile, varInfo, value);
            if ((value instanceof Local) && (index.value > 1024)) {
                this.reportIssue(arkFile, issueReports, stmt, value, severity)
            } else if ((value.getType() instanceof NumberType) && (index.value > 1024)) {
                this.reportIssue(arkFile, issueReports, stmt, value, severity)
            }
        }

    }

    private traverseScope(parentScope: Scope, scopes: Scope[]) {
        scopes.push(parentScope);
        if (parentScope.childScopeList.length !== 0) {
            for (let child of parentScope.childScopeList) {
                this.traverseScope(child, scopes);
            }
        }
    }

    private reportIssue(arkFile: ArkFile, issueReports: IssueReport[], stmt: Stmt, value: Value, severity: number) {
        let filePath = arkFile.getFilePath();
        let lineNum = stmt.getOriginPositionInfo().getLineNo();
        const orgStmt = stmt.getCfg()?.getDeclaringMethod().getBody()?.getStmtToOriginalStmt().get(stmt);
        let startColum = -1;
        let endColum = -1;
        let valStr = '';
        let srgStmtStr = '';
        let orgStmtColum = -1;
        if (orgStmt !== undefined) {
            let orgStmtStr = orgStmt.toString();
            let orgStmtColumn = orgStmt.getOriginPositionInfo().getColNo();
            valStr = NumberUtils.getOriginalValueText(value);
            startColum = this.getRealStartColum(filePath, lineNum, orgStmtColumn, orgStmtStr, valStr, stmt);
            if (startColum === -1) {
                logger.info('find sparse array, but can not get startColum');
                return;
            }
            endColum = startColum + valStr.length - 1;
            this.issueColumnInTs.get(filePath)?.push(lineNum + '%' + startColum);
        }
        filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        let fixKey = lineNum + '%' + startColum + '%' + endColum + '%' + this.ruleId;
        let mergeKey = filePath + '%' + fixKey + '%' + this.description;
        let defects = new Defects(lineNum, startColum, this.description, severity, this.ruleId, mergeKey, this.ruleDocPath, true, false, false, fixKey);
        let issueReport = new IssueReport(filePath, defects);
        issueReports.push(issueReport);
    }

    private getRealStartColum(filePath: string, lineNum: number, orgStmtColumn: number, orgStmtStr: string, valStr: string, stmt: Stmt) {
        let startColumn = -1;
        if (!(stmt instanceof ArkAssignStmt)) {
            return -1;
        }
        let fullStmtValue: SparseArrayValue = this.getFullStmtValue(stmt, valStr);
        let realStmtStr = fullStmtValue.fulStmtStr;
        let tmpOrgStmtStr = orgStmtStr;
        while (tmpOrgStmtStr.includes(realStmtStr)) {
            let fullStmtStartColumn = tmpOrgStmtStr.indexOf(realStmtStr);
            startColumn = orgStmtColumn + fullStmtStartColumn + fullStmtValue.baseStr.length;
            if (!this.hasReported(filePath, lineNum, startColumn)) {
                break;
            }
            tmpOrgStmtStr = tmpOrgStmtStr.replace(realStmtStr, '-'.repeat(realStmtStr.length))
        }
        return startColumn;
    }

    private hasReported(filePath: string, lineNum: number, startColumn: number): boolean {
        let targetIssue = this.issueColumnInTs.get(filePath)?.find(
            (lineCol) => lineCol === lineNum + '%' + startColumn
        );
        return targetIssue !== undefined;
    }

    private getFullStmtValue(stmt: ArkAssignStmt, valStr: string): SparseArrayValue {
        let rightOp = stmt.getRightOp();
        if (rightOp instanceof ArkNewArrayExpr) {
            return new SparseArrayValue(SparseArrayType.NEW_ARRAY, 'new Array(', valStr);
        }
        if (rightOp instanceof ArkArrayRef) {
            let base = rightOp.getBase();
            if (base instanceof Local) {
                let baseStr = base.toString();
                return new SparseArrayValue(SparseArrayType.ARRAY_RIGHT, baseStr + '[', valStr.split(' ')[0]);
            }
        }
        let leftOp = stmt.getLeftOp();
        if (leftOp instanceof ArkArrayRef) {
            let base = leftOp.getBase();
            if (base instanceof Local) {
                let baseStr = base.toString();
                return new SparseArrayValue(SparseArrayType.ARRAY_LEFT, baseStr + '[', valStr.split(' ')[0]);
            }
        }
        return new SparseArrayValue(SparseArrayType.UNKNOWN, '', valStr);
    }
}