import { router } from '@kit.ArkUI';
import {
  ResourceJSBundleProvider,
  RNOHCoreContext,
  RNInstance,
  RNComponentContext,
  RNOHContext,
  buildRNComponentForTag,
} from '@rnoh/react-native-openharmony';

import { createRNPackages, LoadManager, buildCustomComponent } from '../rn';
import { arkTsComponentNames } from '../rn/LoadBundle';
import { createRNController } from './common';

@Entry
@Component
struct Index {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined

  async prepareRNEnv(): Promise<Map<string, RNInstance>> {
    this.rnohCoreContext = AppStorage.get('RNOHCoreContext');
    let instanceMap: Map<string, RNInstance> = new Map();

    const cpInstance: RNInstance = await this.rnohCoreContext!.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: true,
      arkTsComponentNames: arkTsComponentNames
    }
    );
    const ctxCp: RNComponentContext = new RNComponentContext(
      RNOHContext.fromCoreContext(this.rnohCoreContext!, cpInstance),
      wrapBuilder(buildCustomComponent),
      wrapBuilder(buildRNComponentForTag),
      new Map()
    );

    LoadManager.cpInstance = cpInstance;
    await cpInstance.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager,
      'bundle/bundle.harmony.js'));
    instanceMap.set('CPReactInstance', cpInstance);

    return instanceMap;
  }

  aboutToAppear() {
    this.prepareRNEnv().then((instanceMap: Map<string, RNInstance>) => {
      createRNController('cp', this.getUIContext());
    });
  }

  aboutToDisappear() {
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.SpaceEvenly }) {
      Button('进入RN页面').width('100%').onClick(() => {
        router.push({ url: 'pages/RNPage' })
      })
    }
    .padding({ left: 20, right: 20, top: 20 })
    .margin({ top: 32 })
    .height(800)
    .width(400)
  }
}