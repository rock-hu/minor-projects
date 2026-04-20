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
import { ArkClass, ArkFile, ArkInstanceFieldRef, Constant, FunctionType, MethodSignature, Scene, Stmt,
    ViewTreeNode} from "@ArkAnalyzer/src";
import { BaseChecker } from "../BaseChecker";
import { CheckerStorage } from "../../object/CheckerStorage";
import { Defects } from "../../object/Defects";
import { getInvokeExprFromStmt } from "src/common/checkUtils";
import { IssueReport } from "../../object/IssueReport";
import LoggerFactory from "../../utils/LoggerFactory";
import { Rule } from "../../object/Rule";
import { SceneUtils } from "../../utils/SceneUtils";

let logger = LoggerFactory.getLogger('UnionTypeArrayCheck');

export default class AvoidEmptyCallbackCheck extends BaseChecker {
    private gSeverity = -1;
    private readonly STMTSLENGTH = 2;
    private readonly ruleId = '@performance/avoid-empty-callback-check';
    private readonly ruleDocPath = 'docs/avoid-empty-callback-check.md';
    private readonly description = 'Do not set empty system callback listeners.';
    private readonly enentSet: Set<string> = new Set<string>(['onTouch', 'onItemDragMove', 'onDragMove', 'onMouse',
        'onVisibleAreaChange', 'onAreaChange', 'onDidScroll', 'onActionUpdate', 'onClick']);
    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        logger.debug(`Start AvoidEmptyCallbackCheck.`);
        this.gSeverity = rule ? rule.alert : 3;
        let issueReport: IssueReport[] = [];
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
        if (!arkClass.hasViewTree()) {
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
        this.onClickOperation(viewtreeRoot, scene, arkClass, issueReports);
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

    public onClickOperation(viewtreeRoot: ViewTreeNode, scene: Scene, arkClass: ArkClass,
        issueReports: IssueReport[]): void {
        for (let [key, vals] of viewtreeRoot.attributes) {
            if (this.enentSet.has(key)) {
                this.onClickCheck(key, vals, scene, arkClass, issueReports);
            }
        }
    }

    private onClickCheck(key: string, stmts: [Stmt, (Constant | ArkInstanceFieldRef | MethodSignature)[]],
        scene: Scene, arkClass: ArkClass, issueReports: IssueReport[]): void {
        let stmt = stmts[0];
        let invokeExpr = getInvokeExprFromStmt(stmt);
        if (!invokeExpr) {
            return;
        }
        let arg = invokeExpr.getArg(0);
        let type = arg.getType();
        if (!(type instanceof FunctionType)) {
            return;
        }
        let methodSignature = type.getMethodSignature();
        let method = scene.getMethod(methodSignature);
        if (method === null) {
            return;
        }
        let methodStmts = method.getBody()?.getCfg()?.getStmts();
        if (!methodStmts || methodStmts.length !== this.STMTSLENGTH) {
            return;
        }
        let arkFile = arkClass.getDeclaringArkFile();
        this.reportIssue(arkFile, issueReports, stmt, key);
        return;
    }

    private reportIssue(arkFile: ArkFile, issueReports: IssueReport[], stmt: Stmt, keyword: string): void {
        let filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        let originalPosition = stmt.getOriginPositionInfo();
        let line = originalPosition.getLineNo();
        let startColumn = -1;
        let endColumn = -1;
        let orgStmtStr = stmt.getOriginalText();
        if (!orgStmtStr || orgStmtStr.length === 0) {
            return;
        }
        let originalTexts = orgStmtStr.split('\n');
        let lineCount = -1;
        for (let [index, originalText] of originalTexts.entries()) {
            lineCount++;
            if (!originalText.includes(keyword)) {
                continue;
            }
            if (index === 0) {
                startColumn = originalText.indexOf(keyword) + originalPosition.getColNo();
            } else {
                startColumn = originalText.indexOf(keyword) + 1;
            }
            break;
        }
        endColumn = startColumn + keyword.length - 1;
        if (startColumn === -1) {
            return;
        }
        let lineNum = line + lineCount;
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