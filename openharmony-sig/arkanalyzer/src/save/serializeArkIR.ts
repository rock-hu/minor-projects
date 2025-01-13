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

import path from 'path';
import fs from 'fs';
import { Command, InvalidArgumentError } from 'commander';
import { PrinterBuilder } from './PrinterBuilder';
import { SceneConfig } from '../Config';
import { Scene } from '../Scene';
import { ArkFile } from '../core/model/ArkFile';
import { JsonPrinter } from './JsonPrinter';
import { Printer } from './Printer';
import { PointerAnalysis } from '../callgraph/pointerAnalysis/PointerAnalysis';

export function buildSceneFromSingleFile(filename: string, verbose: boolean = false): Scene {
    if (verbose) {
        console.log('Building scene...');
    }
    const filepath = path.resolve(filename);
    const projectDir = path.dirname(filepath);
    const config = new SceneConfig();
    config.buildConfig('single-file', projectDir, []);
    config.getProjectFiles().push(filepath);
    const scene = new Scene();
    scene.buildSceneFromProjectDir(config);
    return scene;
}

export function buildSceneFromProjectDir(inputDir: string, verbose: boolean = false): Scene {
    if (verbose) {
        console.log('Building scene...');
    }
    const config = new SceneConfig();
    config.buildFromProjectDir(inputDir);
    const scene = new Scene();
    scene.buildSceneFromProjectDir(config);
    return scene;
}

export function serializeArkFile(arkFile: ArkFile, output?: string): void {
    let filename = output;
    if (filename === undefined) {
        const outputDir = path.join(arkFile.getProjectDir(), '..', 'output');
        filename = path.join(outputDir, arkFile.getName() + '.json');
    }
    fs.mkdirSync(path.dirname(filename), { recursive: true });
    let printer: Printer = new JsonPrinter(arkFile);
    const fd = fs.openSync(filename, 'w');
    fs.writeFileSync(fd, printer.dump());
    fs.closeSync(fd);
}

export function serializeScene(
    scene: Scene,
    outDir: string,
    verbose: boolean = false,
): void {
    let files = scene.getFiles();
    console.log(`Serializing Scene with ${files.length} files to '${outDir}'...`);
    for (let f of files) {
        let filepath = f.getName();
        let outPath = path.join(outDir, filepath + '.json');
        if (verbose) {
            console.log(`Serializing ArkIR for '${filepath}' to '${outPath}'...`);
        }
        serializeArkFile(f, outPath);
    }
    if (verbose) {
        console.log(`All ${files.length} files in scene are serialized`);
    }
}

function serializeSingleTsFile(
    input: string,
    output: string,
    options: any,
): void {
    if (options.verbose) {
        console.log(`Serializing TS file to JSON: '${input}' -> '${output}'`);
    }

    let filepath = path.resolve(input);
    let projectDir = path.dirname(filepath);

    const scene = buildSceneFromSingleFile(filepath, options.verbose);

    let files = scene.getFiles();
    if (options.verbose) {
        console.log(`Scene contains ${files.length} files`);
        for (let f of files) {
            console.log(`- '${f.getName()}'`);
        }
    }

    if (options.inferTypes) {
        if (options.verbose) {
            console.log('Inferring types...');
        }
        scene.inferTypes();
        if (options.inferTypes > 1) {
            for (let i = 1; i < options.inferTypes; i++) {
                if (options.verbose) {
                    console.log(`Inferring types one more time (${i + 1} / ${options.inferTypes})...`);
                }
                scene.inferTypes();
            }
        }
    }

    if (options.entrypoint) {
        if (options.verbose) {
            console.log('Generating entrypoint...');
        }
        PointerAnalysis.pointerAnalysisForWholeProject(scene);
    }

    if (options.verbose) {
        console.log('Extracting single ArkFile...');
    }
    if (files.length === 0) {
        console.error(`ERROR: No files found in the project directory '${projectDir}'.`);
        process.exit(1);
    }
    if (files.length > 1) {
        console.error(`ERROR: More than one file found in the project directory '${projectDir}'.`);
        process.exit(1);
    }
    // Note: we explicitly push a single path to the project files (in config),
    //       so we expect there is only *one* ArkFile in the scene.
    let arkFile = scene.getFiles()[0];

    let outPath: string;
    if (fs.existsSync(output) && fs.statSync(output).isDirectory()) {
        outPath = path.join(output, arkFile.getName() + '.json');
    } else if (!fs.existsSync(output) && output.endsWith('/')) {
        outPath = path.join(output, arkFile.getName() + '.json');
    } else {
        outPath = output;
    }

    console.log(`Serializing ArkIR for '${arkFile.getName()}' to '${outPath}'...`);
    let printer = new PrinterBuilder();
    printer.dumpToJson(arkFile, outPath);

    if (options.entrypoint) {
        let arkFile = scene.getFiles()[1];
        let outPath: string;
        if (fs.existsSync(output) && fs.statSync(output).isDirectory()) {
            outPath = path.join(output, arkFile.getName() + '.json');
        } else if (!fs.existsSync(output) && output.endsWith('/')) {
            outPath = path.join(output, arkFile.getName() + '.json');
        } else {
            outPath = path.join(path.dirname(output), arkFile.getName() + '.json');
        }
        console.log(`Serializing entrypoint to '${outPath}'...`);
        printer.dumpToJson(arkFile, outPath);
    }

    if (options.verbose) {
        console.log('All done!');
    }
}

function serializeMultipleTsFiles(
    inputDir: string,
    outDir: string,
    options: any,
): void {
    console.log(`Serializing multiple TS files to JSON: '${inputDir}' -> '${outDir}'`);
    if (fs.existsSync(outDir) && !fs.statSync(outDir).isDirectory()) {
        console.error(`ERROR: Output path must be a directory.`);
        process.exit(1);
    }

    if (options.verbose) {
        console.log('Building scene...');
    }
    let config = new SceneConfig();
    config.buildFromProjectDir(inputDir);
    let scene = new Scene();
    scene.buildSceneFromProjectDir(config);

    let files = scene.getFiles();
    if (options.verbose) {
        console.log(`Scene contains ${files.length} files`);
        files.forEach((f) => console.log(`- '${f.getName()}'`));
    }

    if (options.inferTypes) {
        if (options.verbose) {
            console.log('Inferring types...');
        }
        scene.inferTypes();
        if (options.inferTypes > 1) {
            for (let i = 1; i < options.inferTypes; i++) {
                if (options.verbose) {
                    console.log(`Inferring types one more time (${i + 1} / ${options.inferTypes})...`);
                }
                scene.inferTypes();
            }
        }
    }

    if (options.entrypoint) {
        if (options.verbose) {
            console.log('Generating entrypoint...');
        }
        PointerAnalysis.pointerAnalysisForWholeProject(scene);
        files = scene.getFiles();
    }

    if (options.verbose) {
        console.log('Serializing...');
    }
    let printer = new PrinterBuilder();
    for (let f of files) {
        let filepath = f.getName();
        let outPath = path.join(outDir, filepath + '.json');
        console.log(`Serializing ArkIR for '${filepath}' to '${outPath}'...`);
        printer.dumpToJson(f, outPath);
    }
    console.log('All done!');
}

function serializeTsProject(
    inputDir: string,
    outDir: string,
    options: any,
): void {
    console.log(`Serializing TS project to JSON: '${inputDir}' -> '${outDir}'`);

    if (fs.existsSync(outDir) && !fs.statSync(outDir).isDirectory()) {
        console.error(`ERROR: Output path must be a directory.`);
        process.exit(1);
    }

    const scene = buildSceneFromProjectDir(inputDir, options.verbose);

    if (options.inferTypes) {
        if (options.verbose) {
            console.log('Inferring types...');
        }
        scene.inferTypes();
        if (options.inferTypes > 1) {
            for (let i = 1; i < options.inferTypes; i++) {
                if (options.verbose) {
                    console.log(`Inferring types one more time (${i + 1} / ${options.inferTypes})...`);
                }
                scene.inferTypes();
            }
        }
    }

    if (options.entrypoint) {
        if (options.verbose) {
            console.log('Generating entrypoint...');
        }
        PointerAnalysis.pointerAnalysisForWholeProject(scene);
    }

    serializeScene(scene, outDir, options.verbose);

    if (options.verbose) {
        console.log('All done!');
    }
}

function myParseInt(value: string, _previous: number): number {
    const parsedValue = parseInt(value, 10);
    if (isNaN(parsedValue)) {
        throw new InvalidArgumentError('Must be a number.');
    }
    if (parsedValue < 1) {
        throw new InvalidArgumentError('Must be greater than 0.');
    }
    return parsedValue;
}

export const program = new Command()
    .name('serializeArkIR')
    .description('Serialize ArkIR for TypeScript files or projects to JSON')
    .argument('<input>', 'Input file or directory')
    .argument('<output>', 'Output file or directory')
    .option('-m, --multi', 'Flag to indicate the input is a directory', false)
    .option('-p, --project', 'Flag to indicate the input is a project directory', false)
    .option('-t, --infer-types [times]', 'Infer types in the ArkIR', myParseInt)
    .option('-e, --entrypoint', 'Generate entrypoint for the files', false)
    .option('-v, --verbose', 'Verbose output', false)
    .action((input: any, output: any, options: any) => {

        // Check for invalid combinations of flags
        if (options.multi && options.project) {
            console.error(`ERROR: You cannot provide both the '-m' and '-p' flags.`);
            process.exit(1);
        }

        // Ensure the input path exists
        if (!fs.existsSync(input)) {
            console.error(`ERROR: The input path '${input}' does not exist.`);
            process.exit(1);
        }

        // Handle the case where the input is a directory
        if (fs.statSync(input).isDirectory() && !(options.multi || options.project)) {
            console.error(`ERROR: If the input is a directory, you must provide the '-p' or '-m' flag.`);
            process.exit(1);
        }

        if (options.project) {
            serializeTsProject(input, output, options);
        } else if (options.multi) {
            serializeMultipleTsFiles(input, output, options);
        } else {
            serializeSingleTsFile(input, output, options);
        }
    });

if (require.main === module) {
    program.parse(process.argv);
}
