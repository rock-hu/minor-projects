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

import type * as ts from 'typescript';
import * as path from 'node:path';
import type { TSCCompiledProgram } from './TSCCompiledProgram';

/**
 * Extracts TSC diagnostics emitted by strict checks.
 * Function might be time-consuming, as it runs second compilation.
 * @param sourceFiles AST of the processed files
 * @param tscDiagnosticsLinter linter initialized with the processed program
 * @returns problems found by TSC, mapped by `ts.SourceFile.fileName` field
 */
export function getTscDiagnostics(
  tscDiagnosticsLinter: TSCCompiledProgram,
  sourceFiles: ts.SourceFile[]
): Map<string, ts.Diagnostic[]> {
  const strictDiagnostics = new Map<string, ts.Diagnostic[]>();
  sourceFiles.forEach((file) => {
    const diagnostics = tscDiagnosticsLinter.getStrictDiagnostics(file.fileName);
    if (diagnostics.length > 0) {
      strictDiagnostics.set(path.normalize(file.fileName), diagnostics);
    }
  });
  return strictDiagnostics;
}
