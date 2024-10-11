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
    ArkAssignStmt, ArkClass, ArkFile, ArkInstanceInvokeExpr, ArkMethod, ArkStaticInvokeExpr,
    Constant, FunctionType, Local, MethodSignature, Scene, Stmt, Value
} from "@ArkAnalyzer/src";
import { BaseChecker } from "../BaseChecker";
import { CheckerStorage } from "../../object/CheckerStorage";
import { Defects } from "../../object/Defects";
import { getInvokeExprFromStmt } from "src/common/checkUtils";
import { IssueReport } from "../../object/IssueReport";
import { Rule } from "../../object/Rule";
import { SceneUtils } from "../../utils/SceneUtils";
import LoggerFactory from "../../utils/LoggerFactory";

let logger = LoggerFactory.getLogger('TimezoneInterfaceCheck');

export default class TimezoneInterfaceCheck extends BaseChecker {
    private gFilePath = '';
    private gSeverity = -1;
    private readonly ruleId = '@performance/timezone-interface-check';
    private readonly ruleDocPath = 'docs/timezone-interface-check.md';
    private readonly description = 'Suggestion: Use I18n APIs correctly to obtain or set the time zone.';
    private readonly setTimeZoneSignature = '@ohosSdk/api/@ohos.i18n.d.ts: i18n.Calendar.setTimeZone(string)';
    private readonly thirdPartySignature: string[] = [
        '@ohosSdk/api/@ohos.systemDateTime.d.ts: systemDateTime._DEFAULT_ARK_CLASS.setTimezone()',
        '@moment/@hview/moment.d.ts: moment.Moment.utcOffset(number|string, boolean)'];
    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        this.gSeverity = rule ? rule.alert : 3;
        let issueReport: IssueReport[] = [];
        let arkFiles = SceneUtils.getArkFile(filePath, scene);

        for (let arkFile of arkFiles) {
            this.gFilePath = arkFile.getFilePath();
            for (let clazz of arkFile.getClasses()) {
                this.classProcess(clazz, scene, issueReport);
            }
            for (let namespace of arkFile.getNamespaces()) {
                for (let clazz of namespace.getClasses()) {
                    this.classProcess(clazz, scene, issueReport);
                }
            }
        }
        return issueReport;
    }

    public classProcess(arkClass: ArkClass, scene: Scene, issueReports: IssueReport[]): void {
        let arkMethods = arkClass.getMethods();
        for (let arkMethod of arkMethods) {
            let busyMethods = new Set<MethodSignature>();
            this.findSymbolInMethod(arkMethod, scene, busyMethods, issueReports);
        }
    }

    public findSymbolInMethod(arkMethod: ArkMethod, scene: Scene, busyMethods: Set<MethodSignature>,
        issueReports: IssueReport[]): void {
        const stmts = arkMethod.getBody()?.getCfg()?.getStmts();
        if (!stmts) {
            return;
        }
        const curMethodSignature = arkMethod.getSignature();
        busyMethods.add(curMethodSignature);
        for (let stmt of stmts) {
            const invokeExpr = getInvokeExprFromStmt(stmt);
            if (!invokeExpr) {
                continue;
            }
            const invokeSignature = invokeExpr.getMethodSignature();
            let invokeSignatureStr = invokeSignature.toString();
            if (busyMethods.has(invokeSignature) ||
                invokeSignatureStr.includes('@_UnknownProjectName/_UnknowFileName')) {
                continue;
            }
            let clazz = arkMethod.getDeclaringArkClass();
            if (invokeSignatureStr === this.setTimeZoneSignature) {
                this.setTimeCheck(stmt, issueReports, clazz);
            } else if (this.thirdPartySignature.includes(invokeSignatureStr)) {
                let invokeMethodName = invokeSignature.getMethodSubSignature().getMethodName();
                this.reportIssue(clazz.getDeclaringArkFile(), issueReports, stmt, invokeMethodName);
            } else {
                this.findSymbolInInvokeStmt(stmt, scene, busyMethods, issueReports);
                let invokeMethod = scene.getMethod(invokeSignature);
                if (invokeMethod === null) {
                    continue;
                }
                let filepath = invokeMethod.getDeclaringArkFile().getFilePath();
                if (filepath != this.gFilePath) {
                    continue;
                }
                this.findSymbolInMethod(invokeMethod, scene, busyMethods, issueReports);
            }
        }
        busyMethods.delete(curMethodSignature);
    }

    public setTimeCheck(stmt: Stmt, issueReports: IssueReport[], clazz: ArkClass): void {
        if (this.isDefaultParameter(stmt)) {
            return;
        }
        const fieldNameZone = 'zone_offset';
        let zoneStmt = this.getFieldStmt(stmt, fieldNameZone);
        if (!zoneStmt) {
            return;
        }
        const fieldNameDst = 'dst_offset';
        if (!this.getFieldStmt(stmt, fieldNameDst)) {
            this.reportIssue(clazz.getDeclaringArkFile(), issueReports, zoneStmt, fieldNameZone);
        }
    }

    public findSymbolInInvokeStmt(stmt: Stmt, scene: Scene, busyMethods: Set<MethodSignature>,
        issueReports: IssueReport[]): void {
        let invokeArgvs = getInvokeExprFromStmt(stmt)?.getArgs();
        if (invokeArgvs) {
            this.findSymbolInArgs(invokeArgvs, scene, busyMethods, issueReports);
        }
    }

    public findSymbolInArgs(invokeArgvs: Value[], scene: Scene,
        busyMethods: Set<MethodSignature>, issueReports: IssueReport[]): void {
        for (let argv of invokeArgvs) {
            let type = argv.getType();
            if (!(type instanceof FunctionType)) {
                continue;
            }
            let methodSignature = type.getMethodSignature();
            let anonymousMethod = scene.getMethod(methodSignature);
            if (anonymousMethod === null || busyMethods.has(anonymousMethod.getSignature())) {
                continue;
            }
            let filePath = anonymousMethod.getDeclaringArkFile().getFilePath();
            if (filePath != this.gFilePath) {
                continue;
            }
            this.findSymbolInMethod(anonymousMethod, scene, busyMethods, issueReports);
        }
    }

    public getFieldStmt(stmt: Stmt, fieldName: string): Stmt | undefined {
        let invokeExpr = getInvokeExprFromStmt(stmt);
        if (!(invokeExpr instanceof ArkInstanceInvokeExpr)) {
            return undefined;
        }
        let base = invokeExpr.getBase();
        if (!(base instanceof Local)) {
            return undefined;
        }
        let stmts = base.getUsedStmts();
        for (let usedStmt of stmts) {
            const methodNameGet = 'get';
            let usedInvokeExpr = getInvokeExprFromStmt(usedStmt);
            if (!(usedInvokeExpr instanceof ArkInstanceInvokeExpr)) {
                continue;
            }
            let methodName = usedInvokeExpr.getMethodSignature().getMethodSubSignature().getMethodName();
            if (methodName !== methodNameGet) {
                continue;
            }
            let arg = usedInvokeExpr.getArg(0);
            if (!(arg instanceof Constant)) {
                continue
            }
            if (arg.getValue() === fieldName) {
                return usedStmt;
            }
        }
        return undefined;
    }

    public isDefaultParameter(stmt: Stmt): boolean {
        let arg = getInvokeExprFromStmt(stmt)?.getArg(0);
        if (!arg || !(arg instanceof Local)) {
            return false;
        }
        let declaringStmt = arg.getDeclaringStmt();
        if ((!declaringStmt) || !(declaringStmt instanceof ArkAssignStmt)) {
            return false;
        }
        let rightOp = declaringStmt.getRightOp();
        if (!(rightOp instanceof ArkInstanceInvokeExpr)) {
            return false;
        }
        let methodName = rightOp.getMethodSignature().getMethodSubSignature().getMethodName();
        const methodNameGetId = 'getID';
        if (methodName !== methodNameGetId) {
            return false;
        }
        let base = rightOp.getBase();
        if (!(base instanceof Local)) {
            return false;
        }
        let declaringStmtBase = base.getDeclaringStmt();
        if (!declaringStmtBase || !(declaringStmtBase instanceof ArkAssignStmt)) {
            return false;
        }
        let baseRightOp = declaringStmtBase.getRightOp();
        if (!(baseRightOp instanceof ArkStaticInvokeExpr)) {
            return false;
        }
        const baseTimeZone = 'getTimeZone';
        let baseMethodSignature = baseRightOp.getMethodSignature();
        let baseMethodName = baseMethodSignature.getMethodSubSignature().getMethodName();
        if (baseMethodName !== baseTimeZone) {
            return false;
        }
        const fileNameSdk = 'api\\@ohos.i18n.d.ts';
        let fileName = baseMethodSignature.getDeclaringClassSignature().getDeclaringFileSignature().getFileName();
        if (fileName !== fileNameSdk) {
            return false;
        }
        return true;
    }

    private reportIssue(arkFile: ArkFile, issueReports: IssueReport[], stmt: Stmt | undefined, keyword: string): void {
        let filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        if (filePath !== this.gFilePath || !stmt) {
            return;
        }
        let originalPosition = stmt.getOriginPositionInfo();
        let lineNum = originalPosition.getLineNo();
        let orgStmt = stmt.getCfg()?.getDeclaringMethod().getBody()?.getStmtToOriginalStmt().get(stmt);
        let startColumn = -1;
        let endColumn = -1;
        if (orgStmt !== undefined) {
            let orgStmtStr = orgStmt.toString();
            startColumn = originalPosition.getColNo() + orgStmtStr.indexOf(keyword);
            endColumn = startColumn + keyword.length - 1;
        }
        if (startColumn === -1) {
            return;
        }
        const fixKey = lineNum + '%' + startColumn + '%' + endColumn + '%' + this.ruleId;
        const mergeKey = filePath + '%' + fixKey + '%' + this.description;
        let defects = new Defects(lineNum, startColumn, this.description, this.gSeverity,
            this.ruleId, mergeKey, this.ruleDocPath, true, false, false, fixKey);
        let issueReport = new IssueReport(filePath, defects);
        let result = issueReports.find(issue => issue.defect.fixKey === fixKey);
        if (!result) {
            issueReports.push(issueReport);
        } else {
            if (result.defect.description !== this.description) {
                issueReports.push(issueReport);
            }
        }
    }
}