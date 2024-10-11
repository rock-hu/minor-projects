import UIAbility from '@ohos.app.ability.UIAbility';
import type { RNOHLogger } from '@rnoh/react-native-openharmony/src/main/ets/RNOH/RNOHLogger';
import { StandardRNOHLogger } from '@rnoh/react-native-openharmony/src/main/ets/RNOH/RNOHLogger';
import { RNOHCoreContext } from "@rnoh/react-native-openharmony/src/main/ets/RNOH/RNOHCoreContext"
import {
  RNInstanceRegistry,
} from '@rnoh/react-native-openharmony/ts'
import window from '@ohos.window';
import Want from '@ohos.app.ability.Want';
import { RNInstancesCoordinator } from "@rnoh/react-native-openharmony/src/main/ets/RNOH/RNInstancesCoordinator"
import { HttpClient, DefaultHttpClient } from "@rnoh/react-native-openharmony/src/main/ets/HttpClient"
import { EnvironmentCallback } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import common from '@ohos.app.ability.common';

export class UPRNSDKImpl{
  private context:common.UIAbilityContext | null = null;
  protected isDebugModeEnabled_: boolean = true
  private rnInstancesCoordinator!: RNInstancesCoordinator
  private logger!: RNOHLogger
  private rnInstanceRegistry: RNInstanceRegistry | null = null;

  constructor(context: common.UIAbilityContext, want: Want) {
    this.context = context;
    let envCallback: EnvironmentCallback = {
      onConfigurationUpdated(config) {
        let fontSizeScale = config.fontSizeScale
        AppStorage.setOrCreate('fontSizeScale', fontSizeScale)
      },
      onMemoryLevel(level) {
        console.log('onMemoryLevel level: ${level}');
      }
    };
    try {
      let applicationContext = this.context.getApplicationContext();
      let callbackId = applicationContext.on('environment', envCallback);
      console.log(`callbackId: ${callbackId}`);
    } catch (paramError) {
      console.error(`error: ${(paramError as BusinessError).code}, ${(paramError as BusinessError).message}`);
    }
    this.logger = this.createLogger().clone("RNAbility")
    this.rnInstancesCoordinator = RNInstancesCoordinator.create({
      fontSizeScale: this.context.config.fontSizeScale,
      logger: this.createLogger(),
      uiAbilityContext: this.context,
      defaultBackPressHandler: () => {
        this.defaultBackPressHandler()
      },
    }, {
      launchURI: want.uri, onGetPackagerClientConfig: (buildMode) => buildMode === "DEBUG" ? {
        host: "localhost",
        port: 8081
      } : undefined,
      defaultHttpClient: this.onCreateDefaultHttpClient()
    })
    AppStorage.setOrCreate('RNOHCoreContext', this.rnInstancesCoordinator.getRNOHCoreContext())
  }

  protected createLogger(): RNOHLogger {
    return new StandardRNOHLogger((err) => {
      this.rnInstancesCoordinator?.getRNOHCoreContext().reportRNOHError(err)
    });
  }

  onBackPress() {
    const stopTracing = this.logger.clone("onBackPress").startTracing()
    this.rnInstanceRegistry?.forEach((rnInstance) => rnInstance.onBackPress())
    stopTracing()
    return true;
  }
  /**
   * Invoked when the React application doesn't handle the device back press.
   */
  protected defaultBackPressHandler() {
    this.context?.terminateSelf()
  }

  /**
   * @returns HttpClient shared by RNInstances
   */
  protected onCreateDefaultHttpClient(): undefined | HttpClient {
    return new DefaultHttpClient({logger: this.logger});
  }

}
