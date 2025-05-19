/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNPackage,
  DescriptorWrapperFactoryByDescriptorType,
  DescriptorWrapperFactoryByDescriptorTypeCtx,
  UITurboModuleFactory,
  UITurboModuleContext,
  WorkerTurboModuleFactory,
  WorkerTurboModuleContext,
} from '@rnoh/react-native-openharmony';
import { RNC, TM } from "@rnoh/react-native-openharmony/generated/ts"
import { SampleTurboModule } from './SampleTurboModule';
import { SampleWorkerTurboModule } from './SampleWorkerTurboModule';
import { RNC as RNC2, TM as TM2 } from "./generated"
import { SampleLibGenTurboModule } from "./SampleLibGenTurboModule"

class SampleTurboModulesFactory extends UITurboModuleFactory {
  createTurboModule(name: string) {
    if (name === 'SampleTurboModule' || name === TM.GeneratedSampleTurboModule.NAME || name === TM.GeneratedSampleTurboModule2.NAME) {
      return new SampleTurboModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === 'SampleTurboModule' || name === TM.GeneratedSampleTurboModule.NAME || name === TM.GeneratedSampleTurboModule2.NAME;
  }
}

class SampleWorkerTurboModulesFactory extends WorkerTurboModuleFactory {
  createTurboModule(name: string) {
    if (name === TM.SampleWorkerTurboModule.NAME) {
      return new SampleWorkerTurboModule(this.ctx);
    } else if (name === TM2.CodegenLibSampleModule.NAME) {
      return new SampleLibGenTurboModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === TM.SampleWorkerTurboModule.NAME || name === TM2.CodegenLibSampleModule.NAME
  }
}

export class SamplePackage extends RNPackage {
  createUITurboModuleFactory(ctx: UITurboModuleContext) {
    return new SampleTurboModulesFactory(ctx);
  }

  createWorkerTurboModuleFactory(ctx: WorkerTurboModuleContext) {
    return new SampleWorkerTurboModulesFactory(ctx);
  }

  createDescriptorWrapperFactoryByDescriptorType(ctx: DescriptorWrapperFactoryByDescriptorTypeCtx): DescriptorWrapperFactoryByDescriptorType {
    const result: DescriptorWrapperFactoryByDescriptorType = {}
    result[RNC.GeneratedSampleViewArkTS.NAME] = (ctx2) => new RNC.GeneratedSampleViewArkTS.DescriptorWrapper(ctx2.descriptor)
    result[RNC2.CodegenLibArkTSSampleComponent.NAME] = (ctx2) => new RNC2.CodegenLibArkTSSampleComponent.DescriptorWrapper(ctx2.descriptor)
    return result
  }
}
