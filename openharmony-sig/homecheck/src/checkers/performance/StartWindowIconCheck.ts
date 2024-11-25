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
import { fetchDependenciesFromFile, Scene } from "@ArkAnalyzer/src";
import { IssueReport } from "../../object/IssueReport";
import { Rule } from "../../object/Rule";
import { BaseChecker } from "../BaseChecker";
import { Defects } from "../../object/Defects";
import fs from "fs";
import {accessSync, constants} from "fs";
import path from "path";
import getSize from "image-size";
import { ISizeCalculationResult } from "image-size/dist/types/interface";

let logger = LoggerFactory.getLogger('StartWindowIconCheck');

export default class StartWindowIconCheck extends BaseChecker {
    private readonly description = 'Keep the startup icon size within 256 x 256 pixels.';
    private readonly ruleId = '@performance/start-window-icon-check';
    private readonly ruleDocPath = 'docs/start-window-icon-check.md';
    private startWindowIconSeverity: number = -1;
    private isComein = false;

    check(scene: Scene, filePath: string, rule?: Rule): IssueReport[] {
        if (this.isComein) {
            return [];
        }
        this.isComein = true;
        this.startWindowIconSeverity = rule ? rule.alert : 3;
        let issueReports: IssueReport[] = [];
        let moduleJson5Files = this.getJson5Files(scene.getRealProjectDir(), ['.json5']);
        for (let filePath of moduleJson5Files) {
            if (filePath.endsWith('module.json5')) {
                const jsonData = fetchDependenciesFromFile(filePath);
                let module = jsonData.module as moduleJson5Module;
                let type = module.type;
                if (type !== 'entry') {
                    continue;
                }
                let mainElement = module.mainElement;
                if (mainElement === undefined) {
                    continue;
                }
                let abilities = module.abilities;
                if (abilities === undefined) {
                    undefined;
                }
                for (let ability of abilities) {
                    this.iconCheckByAbility(ability, mainElement, filePath, issueReports);
                }
            }
        }
        return issueReports;
    }

    private iconCheckByAbility(ability: moduleAbility, mainElement: string, filePath: string, issueReports: IssueReport[]): void {
        let abilityName = ability.name;
        if (abilityName === mainElement) {
            let startWindowIcon: string = ability.startWindowIcon;
            if (startWindowIcon !== undefined && startWindowIcon.startsWith('$media:')) {
                this.iconCheckByIcon(startWindowIcon, filePath, abilityName, issueReports);
            }
        }
    }

    private iconCheckByIcon(startWindowIcon: string, filePath: string, abilityName: string, issueReports: IssueReport[]): void {
        try {
            let iconName = startWindowIcon.substring(startWindowIcon.indexOf(':') + 1);
            let subReadFilePath = filePath.substring(0, filePath.lastIndexOf('src'));
            subReadFilePath = path.join(subReadFilePath, 'src', 'main', 'resources', 'base', 'media');
            if (this.pathExistsSync(subReadFilePath)) {
                for (let name of fs.readdirSync(subReadFilePath)) {
                    if (name.split('.')[0] === iconName) {
                        this.iconCheckByIconPath(subReadFilePath, name, filePath, abilityName, issueReports);
                    }
                }
            }
        } catch (e) {
            logger.warn(`StartWindowIconCheck error`, e);
        }
    }

    private iconCheckByIconPath(subReadFilePath: string, name: string, filePath: string, abilityName: string, issueReports: IssueReport[]): void {
        let isReport = this.iconCheck(path.join(subReadFilePath, name));
        if (isReport) {
            const warnInfo = this.getWarnInfo(filePath, abilityName);
            const fixKey = warnInfo.line + '%' + warnInfo.startCol + '%' + warnInfo.endCol + '%' + this.ruleId
            const mergeKey = filePath + '%' + fixKey + '%' + this.description;
            issueReports.push(new IssueReport(filePath, new Defects(warnInfo.line, warnInfo.startCol, this.description, this.startWindowIconSeverity, this.ruleId, mergeKey,
                this.ruleDocPath, true, false, false, fixKey)));
        }
    }

    private pathExistsSync(path: string): boolean {
        try {
            accessSync(path, constants.F_OK);
            return true;
        } catch (e) {
            return false;
        }
    }

    private getWarnInfo(filePath: string, abilityName: string): WarnInfo {
        let line = 0;
        let isAbilities = false;
        let isAbilityName = false;
        let isTargetsAbility = false;
        let isStartWindowIcon = false;

        let readData = fs.readFileSync(filePath, 'utf8');
        let readLines: string[] = readData.split('\n');
        let warnInfo: WarnInfo = {line: -1, startCol: -1, endCol: -1};
        for (let readLine of readLines) {
            line++;
            let lineData = readLine.split(':');
            if (readLine.includes('"abilities"')) {
                isAbilities = true;
            } else if (isAbilities && lineData[0].includes('"name"')) {
                isAbilityName = true;
                if (lineData[1].includes(abilityName)) {
                    isTargetsAbility = true;
                }
            } else if (isAbilities && lineData[0].includes('"startWindowIcon"')) {
                isStartWindowIcon = true;
                warnInfo.line = line;
                warnInfo.startCol = readLine.indexOf(':') + 2;
                warnInfo.endCol = readLine.lastIndexOf('"') + 1;
            }

            if (isAbilityName && isStartWindowIcon) {
                if (isTargetsAbility) {
                    return warnInfo;
                }
                isAbilityName = false;
                isStartWindowIcon = false;
            }
        }
        return warnInfo;
    }

    private iconCheck(iconPath: string): boolean {
        try {
            let info = this.getImageInfo(iconPath);
            if (info === null) {
                return false;
            }
            const maxSize = 256 * 256;
            if (info.width !== undefined && info.height !== undefined) {
                const iconSize = info.width * info.height;
                if (iconSize > maxSize) {
                    return true;
                }
            }
        } catch (err) {
            logger.warn(`Error for check icon: ${err}`)
        }
        return false;
    }

    private getImageInfo(iconPath: string): ISizeCalculationResult | null {
        try {
            const info = getSize(iconPath);
            return info;
        } catch (err) {
            logger.warn(`Error gettting image info: ${err}`);
            return null;
        }
    }

    private getJson5Files(srcPath: string, exts: string[], filenameArr: string[] = [], visited: Set<string> = new Set<string>()): string[] {
        if (!fs.existsSync(srcPath)) {
            logger.warn('Input directory is not exist, please check!');
            return filenameArr;
        }
        const realSrc = fs.realpathSync(srcPath);
        if (visited.has(realSrc)) {
            return filenameArr;
        }
        visited.add(realSrc);
        let fileNames = fs.readdirSync(realSrc);
        fileNames.forEach((fileName)=>{
            if (fileName !== 'oh_modules' &&
                fileName !== 'node_modules' &&
                fileName !== 'hvigorfile.ts' &&
                fileName !== 'ohosTest' &&
                fileName !== 'build'
            ) {
                const realFile = path.resolve(realSrc, fileName);
                if (fs.statSync(realFile).isDirectory()) {
                    this.getJson5Files(realFile, exts, filenameArr, visited);
                } else if (exts.includes(path.extname(fileName))) {
                    filenameArr.push(realFile);
                }
            }
        });
        return filenameArr;
    }
}

interface moduleJson5Module {
    type: string;
    mainElement: string;
    abilities: moduleAbility[];
}

interface moduleAbility {
    name: string;
    startWindowIcon: string;
}

interface WarnInfo {
    line: number;
    startCol: number;
    endCol: number;
}