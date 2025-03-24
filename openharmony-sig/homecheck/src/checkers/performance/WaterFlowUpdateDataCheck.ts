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

import LoggerFactory from "src/utils/LoggerFactory";
import { ArkAssignStmt, ArkClass, ArkFile, ArkInstanceFieldRef, ArkInstanceInvokeExpr, ArkInvokeStmt, ArkMethod, ArkNewArrayExpr, ClassSignature, Constant, FileSignature, FunctionType, MethodSignature, Scene, Stmt, Value, ViewTreeNode } from "@ArkAnalyzer/src";
import { IssueReport } from "../../object/IssueReport";
import { Rule } from "../../object/Rule";
import { BaseChecker } from "../BaseChecker";
import { SceneUtils } from "../../utils/SceneUtils";
import { CheckerStorage } from "../../object/CheckerStorage";
import { Defects } from "../../object/Defects";
import ViewTreeTool from "src/utils/ViewTreeTool";

let logger = LoggerFactory.getLogger('WaterFlowUpdateDataCheck');

export default class WaterFlowUpdateDataCheck extends BaseChecker {
    private readonly description = 'Preloading data while using WaterFlow can reduce freezing and achieve infinite scrolling.';
    private readonly ruleId = '@performance/waterflow-data-preload-check';
    private readonly ruleDocPath = 'docs/waterflow-data-preload-check.md';
    private readonly WATERFLOW: string = 'WaterFlow';
    private readonly LAZY_FOREACH: string = 'LazyForEach';
    private readonly CREATE: string = 'create';
    private readonly FLOW_ITEM: string = 'FlowItem';
    private readonly ON_APPEAR: string = 'onAppear';
    private waterFlowUpdateSeverity: number = -1;
    private warnInfo: WarnInfo = {line: -1, startCol: -1, endCol: -1, filePath: ''};
    private usedComponentMap: Map<ClassSignature, string> = new Map();
    private viewTreeTool: ViewTreeTool = new ViewTreeTool();

    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        this.waterFlowUpdateSeverity = rule ? rule.alert : 3;
        let issueReports: IssueReport[] = [];
        this.usedComponentMap.clear();
        let arkFiles = SceneUtils.getArkFile(filePath, scene);
        for (let arkFile of arkFiles) {
            let fileSignature = arkFile.getFileSignature();
            for (let clazz of arkFile.getClasses()) {
                this.classProcess(clazz, fileSignature, scene, issueReports);
            }
            for (let namespace of arkFile.getAllNamespacesUnderThisFile()) {
                for (let clazz of namespace.getClasses()) {
                    this.classProcess(clazz, fileSignature, scene, issueReports);
                }
            }
        }
        return issueReports;
    }

    private classProcess(arkClass: ArkClass, fileSignature: FileSignature, scene: Scene, issueReports: IssueReport[]): void {
        if (arkClass.hasViewTree() && !this.viewTreeTool.hasTraverse(arkClass)) {
            this.warnInfo = {line: -1, startCol: -1, endCol: -1, filePath: ''};
            let viewTreeRoot = arkClass.getViewTree()?.getRoot();
            if (!viewTreeRoot) {
                return;
            }
            this.traverseViewTree(viewTreeRoot, fileSignature, scene, issueReports, 0, 0);
        }
    }

    private traverseViewTree(viewTreeRoot: ViewTreeNode, fileSignature: FileSignature, scene: Scene, 
        issueReports: IssueReport[], waterFlowCount: number, lazyForEachCount: number): boolean {
        let isFinded: boolean = false;
        if (viewTreeRoot === undefined) {
            return false;
        }
        let name = viewTreeRoot.name;
        if (name === this.WATERFLOW) {
            for(let [key, vals] of viewTreeRoot.attributes) {
                this.getWarnInfoByAttributes(key, vals);
            }
            waterFlowCount++;
        } else if (name === this.LAZY_FOREACH && waterFlowCount > 0) {
            lazyForEachCount++;
        } else if (name === this.FLOW_ITEM && lazyForEachCount > 0) {
            for(let [key, vals] of viewTreeRoot.attributes) {
                if ([this.ON_APPEAR].includes(key)) {
                    isFinded = this.findSymbolInAppear(vals, scene);
                    break;
                }
            }
        }
        if (name !== this.FLOW_ITEM) {
            if (viewTreeRoot.children.length > 0 && 
                this.traverseViewTreeByChildren(viewTreeRoot, fileSignature, scene, issueReports, waterFlowCount, lazyForEachCount)
            ) {
                isFinded = true;
            }
        }
        if (name === this.WATERFLOW) {
            waterFlowCount--;
            if (waterFlowCount === 0 && !isFinded) {
                this.pushIssueReport(issueReports);
            }
        } else if (name === this.LAZY_FOREACH && waterFlowCount > 0) {
            lazyForEachCount--;
        }
        return isFinded;
    }

    private pushIssueReport(issueReports: IssueReport[]): void {
        if (this.warnInfo.line !== -1 && !this.isExistIssueReport(issueReports)) {
            const fixKey = this.warnInfo.line + '%' + this.warnInfo.startCol + '%' + this.warnInfo.endCol + '%' + this.ruleId
            const mergeKey = this.warnInfo.filePath + '%' + fixKey + '%' + this.description
            let defects = new Defects(this.warnInfo.line, this.warnInfo.startCol, this.description, this.waterFlowUpdateSeverity, this.ruleId, mergeKey,
                this.ruleDocPath, true, false, false, fixKey);
            issueReports.push(new IssueReport(this.warnInfo.filePath, defects));
            this.warnInfo = {line: -1, startCol: -1, endCol: -1, filePath: ''};
        }
    }

    private isExistIssueReport(issueReports: IssueReport[]): boolean {
        for(let issueReport of issueReports) {
            if (issueReport.filePath === this.warnInfo.filePath &&
                issueReport.defect.reportLine === this.warnInfo.line && 
                issueReport.defect.reportColumn === this.warnInfo.startCol) {
                    return true;
                }
        }
        return false;
    }

    private traverseViewTreeByChildren(viewTreeRoot: ViewTreeNode, fileSignature: FileSignature, scene: Scene, 
        issueReports: IssueReport[], waterFlowCount: number, lazyForEachCount: number): boolean {
        let isFinded: boolean = false;
        for(let child of viewTreeRoot.children) {
            let classSignature = child?.signature;
            if (classSignature && classSignature instanceof ClassSignature) {
                if (fileSignature !== classSignature.getDeclaringFileSignature()) {
                    this.usedComponentMap.has(classSignature);
                    continue;
                }
                this.usedComponentMap.set(classSignature, classSignature.getClassName());
            }
            if (this.traverseViewTree(child, fileSignature, scene, issueReports, waterFlowCount, lazyForEachCount)) {
                isFinded = true;
            }
            if (classSignature && classSignature instanceof ClassSignature) {
                this.usedComponentMap.delete(classSignature);
            }
        }
        return isFinded;
    }

    private findSymbolInAppear(vals: [Stmt, (Constant | ArkInstanceFieldRef | MethodSignature)[]], scene: Scene): boolean {
        for(let val of vals) {
            if (val instanceof ArkAssignStmt) {
                let busyMethods = new Set<MethodSignature>();
                return this.findSymbolInStmt(val, scene, busyMethods);
            }
        }
        return false;
    }

    private findSymbolInStmt(stmt: ArkAssignStmt | ArkInvokeStmt, scene: Scene, busyMethods: Set<MethodSignature>): boolean {
        let invokeArgvs = null;
        if (stmt instanceof ArkAssignStmt) {
            let rightOp = stmt.getRightOp();
            if (rightOp instanceof ArkInstanceInvokeExpr) {
                invokeArgvs = rightOp.getArgs();
            }
        } else if (stmt instanceof ArkInvokeStmt) {
            invokeArgvs = stmt.getInvokeExpr().getArgs();
        }
        if (invokeArgvs) {
            return this.findSymbolInArgs(invokeArgvs, scene, stmt, busyMethods);
        }
        return false;
    }

    private findSymbolInArgs(invokeArgvs: Value[], scene: Scene, stmt: ArkAssignStmt | ArkInvokeStmt, busyMethods: Set<MethodSignature>): boolean {
        for(let argv of invokeArgvs) {
            let type = argv.getType();
            if (type instanceof FunctionType) {
                let methodSignature = type.getMethodSignature();
                let anonymousMethod = scene.getMethod(methodSignature);
                if (anonymousMethod !== null && !busyMethods.has(anonymousMethod.getSignature())) {
                    return this.findSymbolInMethod(anonymousMethod, scene, busyMethods);
                } else {
                    logger.warn('Find FunctionType method error');
                }
            }
        }
        return false;
    }

    private findSymbolInMethod(method: ArkMethod, scene: Scene, busyMethods: Set<MethodSignature>): boolean {
        const stmts = method.getBody()?.getCfg()?.getStmts();
        if (!stmts) {
            return false;
        }
        const curMethodSignature = method.getSignature();
        busyMethods.add(curMethodSignature);
        for(let stmt of stmts) {
            if (stmt instanceof ArkInvokeStmt) {
                const invokeSignature = stmt.getInvokeExpr().getMethodSignature();
                if (busyMethods.has(invokeSignature)) {
                    continue;
                }
                const onDataAddStr = '@ohosSdk/component/lazy_for_each.d.ts: DataChangeListener.onDataAdd(number)';
                const onDataAddedStr = '@ohosSdk/component/lazy_for_each.d.ts: DataChangeListener.onDataAdded(number)';
                let invokeSignatureStr = invokeSignature.toString();
                if (invokeSignatureStr === onDataAddStr || invokeSignatureStr === onDataAddedStr) {
                    busyMethods.delete(curMethodSignature);
                    return true;
                }
                let hasTargetsinvokeSignature = this.findSymbolInStmt(stmt, scene, busyMethods);
                if (hasTargetsinvokeSignature) {
                    busyMethods.delete(curMethodSignature);
                    return true;
                }
                let invokeMethod = this.getInvokeMethod(scene, invokeSignature, stmt);
                if (invokeMethod === null) {
                    continue;
                }
                return this.findSymbolInMethod(invokeMethod, scene, busyMethods);
            }
        }
        return false;
    }

    private getInvokeMethod(scene: Scene, invokeSignature: MethodSignature, stmt: ArkAssignStmt | ArkInvokeStmt): ArkMethod | null {
        let arkMethod = scene.getMethod(invokeSignature);
        if (arkMethod !== null) {
            return arkMethod;
        }
        let invokeSignatureName = invokeSignature.getMethodSubSignature().getMethodName();
        let declaringClass = stmt.getCfg()?.getDeclaringMethod().getDeclaringArkClass();
        if (declaringClass === undefined) {
            return null;
        }
        let supperClass = declaringClass.getSuperClass();
        if (supperClass) {
            arkMethod = supperClass.getMethodWithName(invokeSignatureName);
            return arkMethod;
        }
        return null;
    }

    private getWarnInfoByAttributes(key: string, vals: [Stmt, (Constant | ArkInstanceFieldRef | MethodSignature)[]]): void {
        if ([this.CREATE].includes(key)) {
            for(let val of vals) {
                if (val instanceof ArkAssignStmt) {
                    this.getWarnInfo(val);
                    break;
                }
            }
        }
    }

    private getWarnInfo(stmt: ArkAssignStmt): void {
        const arkFile = stmt.getCfg()?.getDeclaringMethod().getDeclaringArkFile();
        if (arkFile === undefined) {
            return;
        }
        this.warnInfo.filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        let originalPosition = stmt.getOriginPositionInfo();
        this.warnInfo.line = originalPosition.getLineNo();
        this.warnInfo.startCol = originalPosition.getColNo();
        this.warnInfo.endCol = this.warnInfo.startCol + this.WATERFLOW.length - 1;
    }


    private reportIssue(arkFile: ArkFile, issueReports: IssueReport[], stmt: Stmt, text: string, severity: number) {
        let filePath = arkFile.getFilePath();
        let lineNum = stmt.getOriginPositionInfo().getLineNo();
        let startColum = stmt.getOriginPositionInfo().getColNo() + text.indexOf(this.description);
        let endColum = stmt.getOriginPositionInfo().getColNo() + text.indexOf('=') - 1;
        filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        const fixKey = lineNum + '%' + startColum + '%' + endColum + '%' + this.ruleId
        const mergeKey = filePath + '%' + fixKey + '%' + this.description
        let defects = new Defects(lineNum, startColum, this.description, severity, this.ruleId, mergeKey,
            this.ruleDocPath, true, false, false, fixKey);
        let issueReport = new IssueReport(filePath, defects);
        issueReports.push(issueReport);
    }
}

interface WarnInfo {
    line: number;
    startCol: number;
    endCol: number;
    filePath: string;
}