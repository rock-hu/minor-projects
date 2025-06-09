/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

import * as ts from 'typescript';
import type { CommandLineOptions } from '../CommandLineOptions';
import { createCompilerHost, readDeclareFiles } from './ResolveSdks';

function getTargetESVersionLib(optionsTarget: ts.ScriptTarget): string[] {
  switch (optionsTarget) {
    case ts.ScriptTarget.ES2017:
      return ['lib.es2017.d.ts'];
    case ts.ScriptTarget.ES2021:
      return ['lib.es2021.d.ts'];
    default:
      return ['lib.es2021.d.ts'];
  }
}

export function formTscOptions(
  cmdOptions: CommandLineOptions,
  overrideCompilerOptions: ts.CompilerOptions
): ts.CreateProgramOptions {
  if (cmdOptions.parsedConfigFile) {
    const options: ts.CreateProgramOptions = {
      rootNames: cmdOptions.parsedConfigFile.fileNames,
      options: cmdOptions.parsedConfigFile.options,
      projectReferences: cmdOptions.parsedConfigFile.projectReferences,
      configFileParsingDiagnostics: ts.getConfigFileParsingDiagnostics(cmdOptions.parsedConfigFile)
    };
    options.options = Object.assign(options.options, overrideCompilerOptions);
    return options;
  }
  const rootNames = cmdOptions.inputFiles.concat(readDeclareFiles(cmdOptions.sdkDefaultApiPath ?? ''));
  const ESVersion = cmdOptions.followSdkSettings ? ts.ScriptTarget.ES2021 : ts.ScriptTarget.Latest;
  const ESVersionLib = cmdOptions.followSdkSettings ? getTargetESVersionLib(ESVersion) : undefined;
  const isCheckJs = !cmdOptions.followSdkSettings;
  const options: ts.CreateProgramOptions = {
    rootNames: rootNames,
    options: {
      target: ESVersion,
      module: ts.ModuleKind.CommonJS,
      allowJs: true,
      checkJs: isCheckJs,
      lib: ESVersionLib
    }
  };
  if (cmdOptions.sdkDefaultApiPath && cmdOptions.arktsWholeProjectPath && cmdOptions.sdkExternalApiPath) {
    options.host = createCompilerHost(
      cmdOptions.sdkDefaultApiPath,
      cmdOptions.sdkExternalApiPath,
      cmdOptions.arktsWholeProjectPath
    );
  }
  options.options = Object.assign(options.options, overrideCompilerOptions);
  return options;
}
