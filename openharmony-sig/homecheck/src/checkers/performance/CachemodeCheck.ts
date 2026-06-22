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
import { AbstractFieldRef, ArkAssignStmt, ArkClass, ArkFile, ArkStaticFieldRef, Local, Scene, Stmt,
    Value, ViewTreeNode} from "@ArkAnalyzer/src";
import { BaseChecker } from "../BaseChecker";
import { CheckerStorage } from "../../object/CheckerStorage";
import { Defects } from "../../object/Defects";
import { getInvokeExprFromStmt } from "src/common/checkUtils";
import { IssueReport } from "../../object/IssueReport";
import LoggerFactory from "../../utils/LoggerFactory";
import { Rule } from "../../object/Rule";
import { SceneUtils } from "../../utils/SceneUtils";
import ViewTreeTool from "../../utils/ViewTreeTool";

let logger = LoggerFactory.getLogger('CachemodeCheck');

export default class CachemodeCheck extends BaseChecker {
    private gSeverity = -1;
    private readonly WEB: string = 'Web';
    private readonly CACHEMODE: string = 'cacheMode';
    private readonly ruleId = '@performance/cachemode-check';
    private readonly ruleDocPath = 'docs/cachemode-check.md';
    private readonly viewTreeTool: ViewTreeTool = new ViewTreeTool();
    private readonly description = 'Suggestion: Do not set the cachemode parameter to CacheMode.Online.';
    private readonly cachemodeSignature = '@ohosSdk/component/web.d.ts: CacheMode.[static]Online';
    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        logger.debug(`Start CachemodeCheck.`);
        this.gSeverity = rule ? rule.alert : 3;
        let issueReport: IssueReport[] = [];0
        let arkFiles = SceneUtils.getArkFile(filePath,scene);

        for (let arkFile of arkFiles) {
            for (let clazz of arkFile.getClasses()) {
                this.classProcess(clazz, scene, issueReport);
            }
            for (let namespace of arkFile.getAllNamespacesUnderThisFile()) {
                for (let clazz of namespace.getClasses()) {
                    this.classProcess(clazz, scene, issueReport);
                }
            }
        }
        return issueReport;
    }

    public classProcess(arkClass: ArkClass, scene: Scene, issueReports: IssueReport[]): void {
        if (!arkClass.hasViewTree() && !this.viewTreeTool.hasTraverse(arkClass)) {
            return;
        }
        const viewtreeRoot = arkClass.getViewTree()?.getRoot();
        if (!viewtreeRoot) {
            return;
        }
        this.traverseViewTree(viewtreeRoot, arkClass, scene, issueReports);
    }

    public traverseViewTree(viewtreeRoot: ViewTreeNode, arkClass: ArkClass, scene: Scene,
        issueReports: IssueReport[]): void {
        if (viewtreeRoot === undefined) {
            return;
        }
        let name = viewtreeRoot.name;
        if (name === this.WEB) {
            this.cacheModeOnlineCheck(viewtreeRoot, arkClass, issueReports);
        }
        if (viewtreeRoot.children.length > 0) {
            for (let child of viewtreeRoot.children) {
                let classSignature = child.signature;
                if (classSignature && child.isCustomComponent()) {
                    continue;
                }
                this.traverseViewTree(child, arkClass, scene, issueReports);
            }
        }
    }

    public cacheModeOnlineCheck(viewtreeRoot: ViewTreeNode, arkClass: ArkClass, issueReports: IssueReport[]): void {
        let vals = viewtreeRoot.attributes.get(this.CACHEMODE);
        if (!vals) {
            return;
        }
        let stmt = vals[0];
        let invokeExpr = getInvokeExprFromStmt(stmt);
        if (!invokeExpr) {
            return;
        }
        let arg = invokeExpr.getArg(0);
        if (!(arg instanceof Local)) {
            return;
        }
        let declaringStmt = arg.getDeclaringStmt();
        if (!(declaringStmt instanceof ArkAssignStmt)) {
            return;
        }
        let rightOp = declaringStmt.getRightOp();
        if (this.traverseField(rightOp, arkClass)) {
            this.reportIssue(arkClass.getDeclaringArkFile(), issueReports, stmt, arg);
        }
    }

    private traverseField(rightOp: Value, arkClass: ArkClass): boolean {
        if (!(rightOp instanceof AbstractFieldRef)) {
            return false;
        }
        let fieldSignature = rightOp.getFieldSignature();
        if (rightOp instanceof ArkStaticFieldRef) {
            let fieldSignatureStr = fieldSignature.toString();
            if (fieldSignatureStr === this.cachemodeSignature) {
                return true;
            }
        } else {
            let field = arkClass.getField(fieldSignature);
            if (!field) {
                return false;
            }
            let initializerStmts = field.getInitializer();
            for (let initializerStmt of initializerStmts) {
                if (!(initializerStmt instanceof ArkAssignStmt)) {
                    continue;
                }
                let rightOpStmt = initializerStmt.getRightOp();
                return this.traverseField(rightOpStmt, arkClass);
            }
        }
        return false;
    }

    private reportIssue(arkFile: ArkFile, issueReports: IssueReport[], stmt: Stmt, arg: Local): void {
        let filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        let fullPosition = stmt.getOperandOriginalPosition(arg);
        if (!fullPosition) {
            return;
        }
        let lineNum = fullPosition.getFirstLine();
        let startColumn = fullPosition.getFirstCol();
        let endColumn = fullPosition.getLastCol() - 1;
        if (lineNum === -1 || startColumn === -1 || endColumn === -1) {
            return;
        }
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