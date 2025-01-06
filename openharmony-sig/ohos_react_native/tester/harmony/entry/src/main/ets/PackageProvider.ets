/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { RNPackageContext, RNOHPackage } from '@rnoh/react-native-openharmony';
import { createRNOHPackages as createRNOHPackagesAutolinking } from "./RNOHPackagesFactory"
import SamplePackage from '@rnoh/sample-package/src/main/ets/SamplePackage';

export function getRNOHPackages(ctx: RNPackageContext): RNOHPackage[] {
  return [
    ...createRNOHPackagesAutolinking(ctx),
    new SamplePackage(ctx), // comment this line out when testing SamplePackage autolinking
  ]
}