/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

// [Start sampleCode]
import { abilityAccessCtrl, common, PermissionRequestResult } from '@kit.AbilityKit';
import { identifier } from '@kit.AdsKit';
import { ButtonModifier } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Entry
@ComponentV2
struct Index {
  @Local oaid: string = '';
  private context: common.UIAbilityContext = this.getUIContext().getHostContext() as common.UIAbilityContext;
  private buttonModifier: ButtonModifier = new ButtonModifier()
    .fontSize(20)
    .fontWeight(FontWeight.Normal)
    .width('90%')
    .margin({ top: 10, bottom: 10 });

  aboutToAppear(): void {
    requestAppTrackingConsentPermission(this.context);
  }

  build() {
    Navigation() {
      Column() {
        Row() {
          Text($r('app.string.oaid_label'))
            .fontSize(20)
          Text(this.oaid)
            .fontSize(14)
            .height(40)
            .borderRadius(20)
            .backgroundColor($r('sys.color.background_secondary'))
            .textAlign(TextAlign.Center)
            .layoutWeight(1)
            .margin({ left: 5 })
            .copyOption(CopyOptions.InApp)
        }
        .justifyContent(FlexAlign.Start)
        .alignItems(VerticalAlign.Center)
        .width('90%')
        .height(50)

        Button($r('app.string.oaid_btn_promise'))
          .attributeModifier(this.buttonModifier)
          .onClick(() => this.getOaidByPromise())

        Button($r('app.string.oaid_btn_asynccallback'))
          .attributeModifier(this.buttonModifier)
          .onClick(() => this.getOaidByCallback())
      }
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.Center)
    }
    .title($r('app.string.oaid_title'))
    .titleMode(NavigationTitleMode.Mini)
    .mode(NavigationMode.Stack)
    .hideBackButton(true)
  }

  private getOaidByPromise(): void {
    identifier.getOAID().then((data: string) => {
      this.oaid = data;
      hilog.info(0x0000, 'testTag', 'Succeeded in getting oaid by promise');
    }).catch((e: BusinessError) => {
      hilog.error(0x0000, 'testTag', `Failed to get oaid by promise. Code is ${e.code}, message is ${e.message}`);
    });
  }

  private getOaidByCallback(): void {
    identifier.getOAID((e: BusinessError, data: string) => {
      if (e.code) {
        hilog.error(0x0000, 'testTag', `Failed to get oaid by callback. Code is ${e.code}, message is ${e.message}`);
      } else {
        this.oaid = data;
        hilog.info(0x0000, 'testTag', 'Succeeded in getting oaid by callback');
      }
    });
  }
}

async function requestAppTrackingConsentPermission(context: Context): Promise<void> {
  let isPermissionGranted: boolean = false;
  try {
    const atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
    const result: PermissionRequestResult =
      await atManager.requestPermissionsFromUser(context, ['ohos.permission.APP_TRACKING_CONSENT']);
    isPermissionGranted = result.authResults[0] === abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED;
  } catch (err) {
    hilog.error(0x0000, 'testTag', `Failed to request permission. Code is ${err.code}, message is ${err.message}`);
  }
  if (isPermissionGranted) {
    hilog.info(0x0000, 'testTag', 'Succeeded in requesting permission');
  } else {
    hilog.error(0x0000, 'testTag', 'Failed to request permission. User rejected');
  }
}
// [End sampleCode]