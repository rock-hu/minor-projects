/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNInstance,
  RNOHCoreContext,
  ResourceJSBundleProvider,
  RNSurface,
  RNComponentContext,
  RNOHContext,
  buildRNComponentForTag,
  RNPackageContext,
  RNPackage,
  ComponentBuilderContext
} from '@rnoh/react-native-openharmony';

@Builder
function buildCustomComponent(ctx: ComponentBuilderContext) {
  Stack() {}
  .position({ x: 0, y: 0 })
}

function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [];
}

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomComponent);

@Entry
@Component
export struct Index {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined
  private instance: RNInstance | undefined;
  private bundlePath = 'bundle.harmony.js'
  private moduleName = 'App'
  @State isBundleReady: boolean = false;

  async aboutToAppear() {
    console.log("MultiSurface=====")
    if (!this.rnohCoreContext) {
      return;
    }

    this.instance = await this.rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: true,
      arkTsComponentNames: []
    });
    await this.instance.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager, this.bundlePath)).then(() => {
      this.isBundleReady = true
      console.log('instance 加载完成' + this.bundlePath)
    })

  }

  build() {
    NavDestination() {
      if (this.isBundleReady) {
      Column() {
        RNSurface({
          surfaceConfig: {
            appKey: this.moduleName,
          },
          ctx: new RNComponentContext(
            RNOHContext.fromCoreContext(this.rnohCoreContext!, this.instance),
            wrappedCustomRNComponentBuilder,
            wrapBuilder(buildRNComponentForTag),
            new Map()
          ),
        })
      }
      .width('100%')
      .height('100%')
      }
    }

  }
}
   