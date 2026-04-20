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
import { BaseChecker } from "../BaseChecker";
import { ArkClass, Scene, Stmt } from "@ArkAnalyzer/src";
import { Rule } from "src/object/Rule";
import { IssueReport } from "src/object/IssueReport";
import { SceneUtils } from "src/utils/SceneUtils";
import { getInvokeExprFromStmt } from "src/common/checkUtils";
import { CheckerStorage } from "src/object/CheckerStorage";
import { Defects } from "src/object/Defects";


let logger = LoggerFactory.getLogger('ForEachArgsCheck');

export default class ForeachArgsCheck extends BaseChecker {
    private readonly ruleId = '@performance/foreach-args-check';
    private readonly ruleDocPath = 'docs/foreach-args-check.md';
    private readonly description = 'Suggestion: Set KeyGenerator for ForEach.';
    private readonly FOREACH_STR = "ForEach";

    /**
     * 检查ArkUI中ForEach组件的第三个参数keyGenerator是否缺省，缺省则建议添加。
     *
     * @param scene Scene对象
     * @param filePath 文件路径
     * @param rule 规则对象（可选）
     * @returns 问题报告列表
     */
    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        const severity = rule ? rule.alert : 3;
        const issueReports: IssueReport[] = [];
        const arkFiles = SceneUtils.getArkFile(filePath, scene);

        for (const arkFile of arkFiles) {
            for (const arkClass of arkFile.getClasses()) {
                this.arkClassProcess(arkClass, issueReports, severity);
            }
            for (const namespace of arkFile.getAllNamespacesUnderThisFile()) {
                for (const arkClass of namespace.getClasses()) {
                    this.arkClassProcess(arkClass, issueReports, severity);
                }
            }
        }
        return issueReports;
    }

    private arkClassProcess(arkClass: ArkClass, issueReports: IssueReport[], severity: number) {
        for (const arkMethod of arkClass.getMethods()) {
            const stmts = arkMethod.getBody()?.getCfg().getStmts() ?? [];
            for (const stmt of stmts) {
                const invokeExpr = getInvokeExprFromStmt(stmt);
                if (!invokeExpr) {
                    continue;
                }
                const methodSign = invokeExpr.getMethodSignature();
                const className = methodSign.getDeclaringClassSignature().getClassName();
                const methodName = methodSign.getMethodSubSignature().getMethodName();
                const argsNum = invokeExpr.getArgs().length;
                if (className === this.FOREACH_STR && methodName === 'create' && argsNum < 3) {
                    this.addIssueReport(stmt, issueReports, this.FOREACH_STR, severity);
                }
            }
        }
    }

    private addIssueReport(stmt: Stmt, issueReports: IssueReport[], name: string, severity: number) {
        const warnInfo = this.getLineAndColumn(stmt, name);
        const fixKey = warnInfo.line + '%' + warnInfo.startCol + '%' + warnInfo.endCol + '%' + this.ruleId;
        const mergeKey = warnInfo.filePath + '%' + fixKey + '%' + this.description;
        issueReports.push(new IssueReport(warnInfo.filePath, new Defects(warnInfo.line, warnInfo.startCol,
            this.description, severity, this.ruleId, mergeKey, this.ruleDocPath, true, false, false, fixKey)));
    }

    private getLineAndColumn(stmt: Stmt, name: string): WarnInfo {
        const originPosition = stmt.getOriginPositionInfo();
        const line = originPosition.getLineNo();
        const arkFile = stmt.getCfg()?.getDeclaringMethod().getDeclaringArkFile();
        if (arkFile) {
            const originText = stmt.getOriginalText()??'';
            let startCol = originPosition.getColNo();
            const pos = originText.indexOf(name);
            if (pos !== -1) {
                startCol += pos;
                const endCol = startCol + name.length - 1;
                const originPath = CheckerStorage.getInstance().getRealFiePath(arkFile);
                return { line, startCol, endCol, filePath: originPath }
            }
        } else {
            logger.debug('originStmt or arkFile is null');
        }
        return { line: -1, startCol: -1, endCol: -1, filePath: '' };
    }
}

interface WarnInfo {
    line: number;
    startCol: number;
    endCol: number;
    filePath: string;
}