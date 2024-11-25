/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

import { RNPackage, TurboModulesFactory, AnyThreadTurboModuleFactory } from '@rnoh/react-native-openharmony/ts';
import { TurboModule, TurboModuleContext, AnyThreadTurboModuleContext } from '@rnoh/react-native-openharmony/ts';
import { SampleTurboModule } from './SampleTurboModule';
import { SampleTurboModule2 } from './SampleTurboModule2';
import { SampleWorkerTurboModule } from './SampleWorkerTurboModule';

class SampleTurboModulesFactory extends TurboModulesFactory {
  createTurboModule(name: string): TurboModule | null {
    if (name === 'SampleTurboModule') {
      return new SampleTurboModule(this.ctx);
    } else if (name === 'SampleTurboModule2') {
      return new SampleTurboModule2(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === 'SampleTurboModule' || name === 'SampleTurboModule2';
  }
}

class SampleWorkerTurboModulesFactory extends AnyThreadTurboModuleFactory {
  createTurboModule(name: string) {
    if (name === "SampleWorkerTurboModule") {
      return new SampleWorkerTurboModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === "SampleWorkerTurboModule"
  }
}

export class SampleTurboModulePackage extends RNPackage {
  createTurboModulesFactory(ctx: TurboModuleContext): TurboModulesFactory {
    return new SampleTurboModulesFactory(ctx);
  }

  createAnyThreadTurboModuleFactory(ctx: AnyThreadTurboModuleContext) {
    return new SampleWorkerTurboModulesFactory(ctx);
  }
}