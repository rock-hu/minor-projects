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

import { IOptions } from '../configs/IOptions';
import { FileUtils } from './FileUtils';

export class UnitTestUtil {
  // The paths in mKeepFileSourceCode are relative paths. Join them with the directory where the obfuscation configuration file is located 
  // to get the absolute path.
  static initKeepPathConfig(customProfiles: IOptions, configPath: string): void {
    if (customProfiles.mKeepFileSourceCode) {
      const keepFileSourceCode = customProfiles.mKeepFileSourceCode;
      const tempKeepSourceOfPaths = new Set<string>();
      const tempkeepFilesAndDependencies = new Set<string>();

      const processPaths = (paths: Set<string>, targetPaths: Set<string>): void => {
          paths.forEach(tempPath => {
              const absPath = FileUtils.getAbsPathBaseConfigPath(configPath, tempPath);
              targetPaths.add(absPath);
          });
      };

      processPaths(keepFileSourceCode.mKeepSourceOfPaths, tempKeepSourceOfPaths);
      processPaths(keepFileSourceCode.mkeepFilesAndDependencies, tempkeepFilesAndDependencies);

      keepFileSourceCode.mKeepSourceOfPaths = tempKeepSourceOfPaths;
      keepFileSourceCode.mkeepFilesAndDependencies = tempkeepFilesAndDependencies;
   }
  }
}
