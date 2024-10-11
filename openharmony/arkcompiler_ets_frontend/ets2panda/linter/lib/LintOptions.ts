/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

import type * as ts from 'typescript';
import type { CommandLineOptions } from './CommandLineOptions';
import type { IncrementalLintInfo } from './IncrementalLintInfo';
import type { ReportAutofixCallback } from './autofixes/ReportAutofixCallback';
import type { IsFileFromModuleCallback } from './IsFileFromModuleCallback';
import type { IsEtsFileCallback } from './IsEtsFileCallback';
import type { TSCCompiledProgram } from './ts-diagnostics/TSCCompiledProgram';

// common options interface, additional fields may be used by plugins
export interface LintOptions {
  cmdOptions: CommandLineOptions;
  tscCompiledProgram: TSCCompiledProgram;
  cancellationToken?: ts.CancellationToken;
  incrementalLintInfo?: IncrementalLintInfo;
  reportAutofixCb?: ReportAutofixCallback;
  isFileFromModuleCb?: IsFileFromModuleCallback;
  isEtsFileCb?: IsEtsFileCallback;
  isEtsFile?: boolean;
  compatibleSdkVersion?: string;
  compatibleSdkVersionStage?: string;
}
