/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// Script for compiling build behavior. It is built in the build plug-in and cannot be modified currently.
export { harTasks } from '@ohos/hvigor-ohos-plugin';
import { harTasks } from '@ohos/hvigor-ohos-plugin';
import { createInnerHarHvigorPlugin } from "@rnoh/inner-hvigor-plugin";

export default {
  system: harTasks,
  plugins: [
    createInnerHarHvigorPlugin()
  ]
}
