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

import fs from 'fs';
import path from 'path';
import { ArkFile } from '../core/model/ArkFile';
import { DotFilePrinter } from './DotPrinter';
import { SourceFilePrinter } from './source/SourceFilePrinter';
import { Printer } from './Printer';
import { JsonPrinter } from './JsonPrinter';

/**
 * @example
 * // dump method IR to ts source
 * let method: Method = xx;
 * let srcPrinter = new SourceMethodPrinter(method);
 * PrinterBuilder.dump(srcPrinter, 'output.ts');
 *
 *
 * // dump method cfg to dot
 * let dotPrinter = new DotMethodPrinter(method);
 * PrinterBuilder.dump(dotPrinter, 'output.dot');
 *
 * // dump project
 * let printer = new PrinterBuilder('output');
 * for (let f of scene.getFiles()) {
 *     printer.dumpToTs(f);
 * }
 *
 * @category save
 */
export class PrinterBuilder {
    outputDir: string;
    constructor(outputDir: string = '') {
        this.outputDir = outputDir;
    }

    public static dump(source: Printer, output: string) {
        fs.writeFileSync(output, source.dump());
    }

    protected getOutputDir(arkFile: ArkFile): string {
        if (this.outputDir === '') {
            return path.join(arkFile.getProjectDir(), '..', 'output');
        } else {
            return path.join(this.outputDir);
        }
    }

    public dumpToDot(
        arkFile: ArkFile,
        output: string | undefined = undefined
    ): void {
        let filename = output;
        if (filename === undefined) {
            filename = path.join(this.getOutputDir(arkFile), arkFile.getName() + '.dot');
        }
        fs.mkdirSync(path.dirname(filename), { recursive: true });

        let printer: Printer = new DotFilePrinter(arkFile);
        PrinterBuilder.dump(printer, filename as string);
    }

    public dumpToTs(
        arkFile: ArkFile,
        output: string | undefined = undefined
    ): void {
        let filename = output;
        if (filename === undefined) {
            filename = path.join(this.getOutputDir(arkFile), arkFile.getName());
        }
        if (path.extname(filename) === '') {
            filename += '.ts';
        }
        fs.mkdirSync(path.dirname(filename), { recursive: true });

        let printer: Printer = new SourceFilePrinter(arkFile);
        PrinterBuilder.dump(printer, filename);
    }

    public dumpToJson(arkFile: ArkFile, output: string | undefined = undefined): void {
        let filename = output;
        if (filename === undefined) {
            filename = path.join(this.getOutputDir(arkFile), arkFile.getName() + '.json');
        }
        fs.mkdirSync(path.dirname(filename), { recursive: true });

        let printer: Printer = new JsonPrinter(arkFile);
        PrinterBuilder.dump(printer, filename);
    }
}
