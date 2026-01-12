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
import { ArkClass, ArkFile, ArkMethod, ArkNamespace, FunctionType, MethodSignature, Scene, Stmt,
    UNKNOWN_PROJECT_NAME, Value} from "@ArkAnalyzer/src";
import { BaseChecker } from "../BaseChecker";
import { CheckerStorage } from "../../object/CheckerStorage";
import { Defects } from "../../object/Defects";
import { getInvokeExprFromStmt } from "src/common/checkUtils";
import { IssueReport } from "../../object/IssueReport";
import LoggerFactory from "../../utils/LoggerFactory";
import { Rule } from "../../object/Rule";
import { SceneUtils } from "../../utils/SceneUtils";

let logger = LoggerFactory.getLogger('CachemodeCheck');

export default class CacheAvplayerCheck extends BaseChecker {
    private gSeverity = -1;
    private readonly gKeyword: string = 'release';
    private readonly ruleId = '@performance/cache-avplayer-check';
    private readonly ruleDocPath = 'docs/cache-avplayer-check.md';
    private readonly importSignature = '@ohosSdk/api/@ohos.multimedia.media.d.ts: ';
    private readonly description = 'Suggestion: cache avplayer instances to shorten the playback start delay.';
    private readonly releaseSignature =
        '@ohosSdk/api/@ohos.multimedia.media.d.ts: media.AVPlayer.release(AsyncCallback<void>)';

    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        logger.debug(`Start CacheAvplayerCheck.`);
        this.gSeverity = rule ? rule.alert : 3;
        let issueReport: IssueReport[] = [];
        let arkFiles = SceneUtils.getArkFile(filePath,scene);

        for (let arkFile of arkFiles) {
            if (!this.importCheck(arkFile)) {
                continue;
            }
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

    public importCheck(arkFile: ArkFile): boolean {
        let importInfos = arkFile.getImportInfos();
        for (let importInfo of importInfos) {
            let lazyExportInfo = importInfo.getLazyExportInfo();
            if (!lazyExportInfo) {
                continue;
            }
            let arkExport = lazyExportInfo.getArkExport();
            if (!(arkExport instanceof ArkNamespace)) {
                continue;
            }
            let DecArkFile = arkExport.getDeclaringArkFile();
            if (!DecArkFile) {
                continue;
            }
            let declaringFileSignature = DecArkFile.getFileSignature();
            if (declaringFileSignature.toString() === this.importSignature) {
                return true;
            }
        }
        return false;
    }

    public classProcess(arkClass: ArkClass, scene: Scene, issueReports: IssueReport[]): void {
        let arkMethods = arkClass.getMethods();
        for (let arkMethod of arkMethods) {
            if (arkMethod.getName() !== 'aboutToDisappear') {
                continue;
            }
            let busyMethods = new Set<MethodSignature>();
            this.findSymbolInMethod(arkMethod, scene, busyMethods, issueReports);
        }
    }

    public findSymbolInMethod(arkMethod: ArkMethod, scene: Scene, busyMethods: Set<MethodSignature>,
        issueReports: IssueReport[]): void {
        const cfgStmts = arkMethod.getBody()?.getCfg()?.getStmts();
        if (!cfgStmts) {
            return;
        }
        const curMethodSignature = arkMethod.getSignature();
        busyMethods.add(curMethodSignature);
        for (let stmt of cfgStmts) {
            const invokeExpr = getInvokeExprFromStmt(stmt);
            if (!invokeExpr) {
                continue;
            }
            const cacheInvokeSignature = invokeExpr.getMethodSignature();
            let invokeSignatureStr = cacheInvokeSignature.toString();
            if (busyMethods.has(cacheInvokeSignature) || 
                invokeSignatureStr.includes('@' + UNKNOWN_PROJECT_NAME + '/' + UNKNOWN_PROJECT_NAME)) {
                    continue;
            }
            let clazz = arkMethod.getDeclaringArkClass();
            if (invokeSignatureStr === this.releaseSignature) {
                this.reportIssue(clazz.getDeclaringArkFile(), issueReports, stmt);
            } else {
                this.findSymbolInInvokeStmt(stmt, scene, busyMethods, issueReports);
                let invokeMethod = scene.getMethod(cacheInvokeSignature);
                if (invokeMethod === null) {
                    continue;
                }
                this.findSymbolInMethod(invokeMethod, scene, busyMethods, issueReports);
            }
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
            if (anonymousMethod !== null && !busyMethods.has(anonymousMethod.getSignature())) {
                this.findSymbolInMethod(anonymousMethod, scene, busyMethods, issueReports);
            } else {
                logger.debug('Find FunctionType method error!');
            }
        }
    }

    private reportIssue(arkFile: ArkFile, issueReports: IssueReport[], stmt: Stmt): void {
        let filePath = CheckerStorage.getInstance().getRealFiePath(arkFile);
        let originalPosition = stmt.getOriginPositionInfo();
        let lineNum = originalPosition.getLineNo();
        let orgStmtStr = stmt.getOriginalText();
        let startColumn = -1;
        let endColumn = -1;
        if (orgStmtStr && orgStmtStr.length !== 0) {
            startColumn = originalPosition.getColNo() + orgStmtStr.indexOf(this.gKeyword);
            endColumn = startColumn + this.gKeyword.length - 1;
        }
        if (startColumn === -1) {
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