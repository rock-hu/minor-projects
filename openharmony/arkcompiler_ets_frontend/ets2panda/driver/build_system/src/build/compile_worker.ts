/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { CompileFileInfo, ModuleInfo } from '../types';
import * as fs from 'fs';
import * as path from 'path';
import { ensurePathExists } from '../utils';
import { KOALA_WRAPPER_PATH_FROM_SDK } from '../pre_define';
import { PluginDriver, PluginHook } from '../plugins/plugins_driver';
import { BuildConfig } from '../types';
import { BUILD_MODE } from '../pre_define';
import { Logger } from '../logger';

process.on('message', (message: {
  taskList: CompileFileInfo[];
  buildConfig: BuildConfig;
  moduleInfos: Array<[string, ModuleInfo]>;
}) => {
  if (!process.send) {
    throw new Error('process.send is undefined. This worker must be run as a forked process.');
  }
  const { taskList, buildConfig, moduleInfos } = message;
  const isDebug = buildConfig.buildMode === BUILD_MODE.DEBUG;

  Logger.getInstance(buildConfig);
  PluginDriver.getInstance().initPlugins(buildConfig);
  const koalaWrapperPath = path.resolve(buildConfig.buildSdkPath, KOALA_WRAPPER_PATH_FROM_SDK);
  let { arkts, arktsGlobal } = require(koalaWrapperPath);

  for (const fileInfo of taskList) {
    let errorStatus = false;
    try {
      ensurePathExists(fileInfo.abcFilePath);
      const source = fs.readFileSync(fileInfo.filePath).toString();

      const ets2pandaCmd = [
        '_', '--extension', 'ets',
        '--arktsconfig', fileInfo.arktsConfigFile,
        '--output', fileInfo.abcFilePath,
      ];
      if (isDebug) {
        ets2pandaCmd.push('--debug-info');
      }
      ets2pandaCmd.push(fileInfo.filePath);

      arktsGlobal.filePath = fileInfo.filePath;
      arktsGlobal.config = arkts.Config.create(ets2pandaCmd).peer;
      arktsGlobal.compilerContext = arkts.Context.createFromString(source);

      PluginDriver.getInstance().getPluginContext().setArkTSProgram(arktsGlobal.compilerContext.program);

      arkts.proceedToState(arkts.Es2pandaContextState.ES2PANDA_STATE_PARSED);
      PluginDriver.getInstance().runPluginHook(PluginHook.PARSED);

      arkts.proceedToState(arkts.Es2pandaContextState.ES2PANDA_STATE_CHECKED);
      PluginDriver.getInstance().runPluginHook(PluginHook.CHECKED);

      arkts.proceedToState(arkts.Es2pandaContextState.ES2PANDA_STATE_BIN_GENERATED);
    } catch (error) {
      errorStatus = true;
      if (error instanceof Error) {
        process.send({
          success: false,
          filePath: fileInfo.filePath,
          error: 'Compile abc files failed.\n' + error.message
        });
      }
    } finally {
      if (!errorStatus) {
        // when error occur,wrapper will destroy context.
        arktsGlobal.es2panda._DestroyContext(arktsGlobal.compilerContext.peer);
      }
      PluginDriver.getInstance().runPluginHook(PluginHook.CLEAN);
      arkts.destroyConfig(arktsGlobal.config);
    }
  }

  process.exit(0);
});