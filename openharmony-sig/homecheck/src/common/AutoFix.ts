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

import {IssueReport} from "../object/IssueReport";
import {ArkFile, FileSignature, PrinterBuilder, Scene, SourceFilePrinter} from "@ArkAnalyzer/src";
import * as path from "node:path";

export function autoFix(issueReport:IssueReport, scene:Scene) {
    // let filePath = issueReport.filePath;
    // let projectName = scene.getProjectName();
    // let realProjectDir = scene.getRealProjectDir();
    let filePath = "C:/work/applications_photos/common/test.ets";
    let projectName = "OpenSource";
    let realProjectDir = "C:/work/applications_photos";

    let fileName = path.relative(realProjectDir, filePath);

    // const fileName = path.basename(filePath);
    let fileSignature = new FileSignature(projectName, fileName);

    let arkFile: ArkFile | null = scene.getFile(fileSignature);
    if (arkFile){
        //todo 修改arkFile

        let filePrinter = new SourceFilePrinter(arkFile);
        PrinterBuilder.dump(filePrinter,"C:\\work\\homecheck\\out\\fixed.ts")
    }
}

// autoFix()