/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { hvigor, HvigorNode, HvigorPlugin } from '@ohos/hvigor';
import { OhosAppContext, OhosHarContext, OhosPluginId } from '@ohos/hvigor-ohos-plugin';
import {
  Logger,
  RealsePrebuildTask,
} from './PrebuiltTask';

class LoggerImpl implements Logger {
  info(message: string): void {
    console.info(message);
  }

  warn(message: string): void {
    console.warn(message);
  }

  error(message: string): void {
    console.error(message);
  }
}

/**
 * This plugin is designed to assist with the pipeline compilation of release versions
 * and bytecode HAR packages for internal use.
 * @returns HvigorPlugin
 */
export function createInnerHarHvigorPlugin(): HvigorPlugin {
  return {
    pluginId: 'rnoh_inner_har',
    apply: (currentNode: HvigorNode) => {
      const isPipeline = hvigor.getParameter().getExtParam('pipeline') === '1';
      if (!isPipeline) {
        return;
      }
      const logger = new LoggerImpl();
      const appContext = currentNode.getParentNode()?.getContext(OhosPluginId.OHOS_APP_PLUGIN) as OhosAppContext;
      if (!appContext) {
        logger.warn('Failed to get OhosAppContext!');
        return;
      }
      
      const buildMode = appContext.getBuildMode();
      const productName = appContext.getCurrentProduct().getProductName();
      const harContext = currentNode.getContext(OhosPluginId.OHOS_HAR_PLUGIN) as OhosHarContext;
      const task = new RealsePrebuildTask(logger, harContext, { buildMode, productName });
      task.run();
    }
  };
}
