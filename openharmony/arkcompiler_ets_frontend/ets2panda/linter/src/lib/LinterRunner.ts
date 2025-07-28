/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import * as fs from 'node:fs';
import * as path from 'node:path';
import * as ts from 'typescript';
import { processSyncErr } from '../lib/utils/functions/ProcessWrite';
import * as qEd from './autofixes/QuasiEditor';
import type { BaseTypeScriptLinter } from './BaseTypeScriptLinter';
import type { CommandLineOptions } from './CommandLineOptions';
import { InteropTypescriptLinter } from './InteropTypescriptLinter';
import type { LinterConfig } from './LinterConfig';
import type { LinterOptions } from './LinterOptions';
import type { LintRunResult } from './LintRunResult';
import { Logger } from './Logger';
import type { ProblemInfo } from './ProblemInfo';
import { ProjectStatistics } from './statistics/ProjectStatistics';
import { generateMigrationStatisicsReport } from './statistics/scan/ProblemStatisticsCommonFunction';
import type { TimeRecorder } from './statistics/scan/TimeRecorder';
import type { createProgramCallback } from './ts-compiler/Compiler';
import { compileLintOptions } from './ts-compiler/Compiler';
import { getTscDiagnostics } from './ts-diagnostics/GetTscDiagnostics';
import { transformTscDiagnostics } from './ts-diagnostics/TransformTscDiagnostics';
import { TypeScriptLinter } from './TypeScriptLinter';
import {
  ARKTS_IGNORE_DIRS_NO_OH_MODULES,
  ARKTS_IGNORE_DIRS_OH_MODULES,
  ARKTS_IGNORE_FILES
} from './utils/consts/ArktsIgnorePaths';
import { EXTNAME_JS, EXTNAME_TS } from './utils/consts/ExtensionName';
import { USE_STATIC } from './utils/consts/InteropAPI';
import { LibraryTypeCallDiagnosticChecker } from './utils/functions/LibraryTypeCallDiagnosticChecker';
import { mergeArrayMaps } from './utils/functions/MergeArrayMaps';
import { clearPathHelperCache, pathContainsDirectory } from './utils/functions/PathHelper';

function prepareInputFilesList(cmdOptions: CommandLineOptions): string[] {
  let inputFiles = cmdOptions.inputFiles.map((x) => {
    return path.normalize(x);
  });
  if (!cmdOptions.parsedConfigFile) {
    return inputFiles;
  }

  inputFiles = cmdOptions.parsedConfigFile.fileNames;
  if (cmdOptions.inputFiles.length <= 0) {
    return inputFiles;
  }

  /*
   * Apply linter only to the project source files that are specified
   * as a command-line arguments. Other source files will be discarded.
   */
  const cmdInputsResolvedPaths = cmdOptions.inputFiles.map((x) => {
    return path.resolve(x);
  });
  const configInputsResolvedPaths = inputFiles.map((x) => {
    return path.resolve(x);
  });
  inputFiles = configInputsResolvedPaths.filter((x) => {
    return cmdInputsResolvedPaths.some((y) => {
      return x === y;
    });
  });

  return inputFiles;
}

export function lint(
  config: LinterConfig,
  timeRecorder: TimeRecorder,
  etsLoaderPath?: string,
  hcResults?: Map<string, ProblemInfo[]>
): LintRunResult {
  if (etsLoaderPath) {
    config.cmdOptions.linterOptions.etsLoaderPath = etsLoaderPath;
  }
  const lintResult = lintImpl(config);
  timeRecorder.endScan();
  return config.cmdOptions.linterOptions.migratorMode ?
    migrate(config, lintResult, timeRecorder, hcResults) :
    lintResult;
}

function lintImpl(config: LinterConfig): LintRunResult {
  const { cmdOptions, tscCompiledProgram } = config;
  const tsProgram = tscCompiledProgram.getProgram();
  const options = cmdOptions.linterOptions;

  // Prepare list of input files for linter and retrieve AST for those files.
  let inputFiles = prepareInputFilesList(cmdOptions);
  inputFiles = inputFiles.filter((input) => {
    return shouldProcessFile(options, input);
  });
  options.inputFiles = inputFiles;
  const srcFiles: ts.SourceFile[] = [];
  for (const inputFile of inputFiles) {
    const srcFile = tsProgram.getSourceFile(inputFile);
    if (srcFile) {
      srcFiles.push(srcFile);
    }
  }

  const tscStrictDiagnostics = getTscDiagnostics(tscCompiledProgram, srcFiles);
  LibraryTypeCallDiagnosticChecker.instance.rebuildTscDiagnostics(tscStrictDiagnostics);
  const lintResult = lintFiles(tsProgram, srcFiles, options, tscStrictDiagnostics);
  LibraryTypeCallDiagnosticChecker.instance.clear();

  if (!options.ideInteractive) {
    lintResult.problemsInfos = mergeArrayMaps(lintResult.problemsInfos, transformTscDiagnostics(tscStrictDiagnostics));
  }

  freeMemory();
  return lintResult;
}

function lintFiles(
  tsProgram: ts.Program,
  srcFiles: ts.SourceFile[],
  options: LinterOptions,
  tscStrictDiagnostics: Map<string, ts.Diagnostic[]>
): LintRunResult {
  const projectStats: ProjectStatistics = new ProjectStatistics();
  const problemsInfos: Map<string, ProblemInfo[]> = new Map();

  TypeScriptLinter.initGlobals();
  InteropTypescriptLinter.initGlobals();
  let fileCount: number = 0;

  for (const srcFile of srcFiles) {
    const linter: BaseTypeScriptLinter = !options.interopCheckMode ?
      new TypeScriptLinter(tsProgram.getTypeChecker(), options, srcFile, tscStrictDiagnostics) :
      new InteropTypescriptLinter(tsProgram.getTypeChecker(), tsProgram.getCompilerOptions(), options, srcFile);
    linter.lint();
    const problems = linter.problemsInfos;
    problemsInfos.set(path.normalize(srcFile.fileName), [...problems]);
    projectStats.fileStats.push(linter.fileStats);
    fileCount = fileCount + 1;
    if (options.ideInteractive) {
      processSyncErr(`{"content":"${srcFile.fileName}","messageType":1,"indicator":${fileCount / srcFiles.length}}\n`);
    }
  }

  return {
    hasErrors: projectStats.hasError(),
    problemsInfos,
    projectStats
  };
}

function migrate(
  initialConfig: LinterConfig,
  initialLintResult: LintRunResult,
  timeRecorder: TimeRecorder,
  hcResults?: Map<string, ProblemInfo[]>
): LintRunResult {
  timeRecorder.startMigration();
  let linterConfig = initialConfig;
  const { cmdOptions } = initialConfig;
  const updatedSourceTexts: Map<string, string> = new Map();
  let lintResult: LintRunResult = initialLintResult;
  const problemsInfosBeforeMigrate = lintResult.problemsInfos;

  for (let pass = 0; pass < (cmdOptions.linterOptions.migrationMaxPass ?? qEd.DEFAULT_MAX_AUTOFIX_PASSES); pass++) {
    const appliedFix = fix(linterConfig, lintResult, updatedSourceTexts, hcResults);
    hcResults = undefined;

    if (!appliedFix) {
      // No fixes were applied, migration is finished.
      break;
    }

    // Re-compile and re-lint project after applying the fixes.
    linterConfig = compileLintOptions(cmdOptions, getMigrationCreateProgramCallback(updatedSourceTexts));
    lintResult = lintImpl(linterConfig);
  }

  // Write new text for updated source files.
  updatedSourceTexts.forEach((newText, fileName) => {
    if (!cmdOptions.linterOptions.noMigrationBackupFile) {
      qEd.QuasiEditor.backupSrcFile(fileName);
    }
    const filePathMap = cmdOptions.linterOptions.migrationFilePathMap;
    const writeFileName = filePathMap?.get(fileName) ?? fileName;
    fs.writeFileSync(writeFileName, newText);
  });

  timeRecorder.endMigration();
  generateMigrationStatisicsReport(lintResult, timeRecorder, cmdOptions.outputFilePath);

  if (cmdOptions.linterOptions.ideInteractive) {
    lintResult.problemsInfos = problemsInfosBeforeMigrate;
  }

  return lintResult;
}

function hasUseStaticDirective(srcFile: ts.SourceFile): boolean {
  if (!srcFile?.statements.length) {
    return false;
  }
  const statements = srcFile.statements;
  return (
    ts.isExpressionStatement(statements[0]) &&
    ts.isStringLiteral(statements[0].expression) &&
    statements[0].expression.getText() === USE_STATIC
  );
}

function fix(
  linterConfig: LinterConfig,
  lintResult: LintRunResult,
  updatedSourceTexts: Map<string, string>,
  hcResults?: Map<string, ProblemInfo[]>
): boolean {
  const program = linterConfig.tscCompiledProgram.getProgram();
  let appliedFix = false;
  // Apply homecheck fixes first to avoid them being skipped due to conflict with linter autofixes
  let mergedProblems: Map<string, ProblemInfo[]> = hcResults ?? new Map();
  mergedProblems = mergeArrayMaps(mergedProblems, lintResult.problemsInfos);
  mergedProblems.forEach((problemInfos, fileName) => {
    const srcFile = program.getSourceFile(fileName);
    if (!srcFile) {
      if (!linterConfig.cmdOptions.homecheck) {
        Logger.error(`Failed to retrieve source file: ${fileName}`);
      }
      return;
    }
    const needToAddUseStatic =
      linterConfig.cmdOptions.linterOptions.arkts2 &&
      linterConfig.cmdOptions.inputFiles.includes(fileName) &&
      !hasUseStaticDirective(srcFile) &&
      linterConfig.cmdOptions.linterOptions.ideInteractive &&
      !qEd.QuasiEditor.hasAnyAutofixes(problemInfos);
    // If nothing to fix or don't need to add 'use static', then skip file
    if (!qEd.QuasiEditor.hasAnyAutofixes(problemInfos) && !needToAddUseStatic) {
      return;
    }
    const qe: qEd.QuasiEditor = new qEd.QuasiEditor(
      fileName,
      srcFile.text,
      linterConfig.cmdOptions.linterOptions,
      undefined,
      linterConfig.cmdOptions.outputFilePath
    );
    updatedSourceTexts.set(fileName, qe.fix(problemInfos, needToAddUseStatic));
    if (!needToAddUseStatic) {
      appliedFix = true;
    }
  });

  return appliedFix;
}

function getMigrationCreateProgramCallback(updatedSourceTexts: Map<string, string>): createProgramCallback {
  return (createProgramOptions: ts.CreateProgramOptions): ts.Program => {
    const compilerHost = createProgramOptions.host || ts.createCompilerHost(createProgramOptions.options, true);
    const originalReadFile = compilerHost.readFile;
    compilerHost.readFile = (fileName: string): string | undefined => {
      const newText = updatedSourceTexts.get(path.normalize(fileName));
      return newText || originalReadFile(fileName);
    };
    createProgramOptions.host = compilerHost;
    return ts.createProgram(createProgramOptions);
  };
}

export function shouldProcessFile(options: LinterOptions, fileFsPath: string): boolean {
  if (!options.checkTsAndJs && (path.extname(fileFsPath) === EXTNAME_TS || path.extname(fileFsPath) === EXTNAME_JS)) {
    return false;
  }

  if (
    ARKTS_IGNORE_FILES.some((ignore) => {
      return path.basename(fileFsPath) === ignore;
    })
  ) {
    return false;
  }

  if (
    ARKTS_IGNORE_DIRS_NO_OH_MODULES.some((ignore) => {
      return pathContainsDirectory(path.resolve(fileFsPath), ignore);
    })
  ) {
    return false;
  }

  return (
    !pathContainsDirectory(path.resolve(fileFsPath), ARKTS_IGNORE_DIRS_OH_MODULES) ||
    !!options.isFileFromModuleCb?.(fileFsPath)
  );
}

function freeMemory(): void {
  clearPathHelperCache();
}
