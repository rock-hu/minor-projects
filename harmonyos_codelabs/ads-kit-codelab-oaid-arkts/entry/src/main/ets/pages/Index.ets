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
import { BusinessError } from '@kit.BasicServicesKit';
import { identifier } from '@kit.AdsKit';
import { abilityAccessCtrl, common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Entry
@Component
struct Index {
  // Obtained OAID.
  @State oaid: string = '';
  private context: common.UIAbilityContext = this.getUIContext().getHostContext() as common.UIAbilityContext;

  aboutToAppear() {
    this.requestOAIDTrackingConsentPermissions(this.context);
  }

  requestOAIDTrackingConsentPermissions(context: common.Context): void {
    // Request Activity tracking for this app from users when they access a page.
    const atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
    try {
      atManager.requestPermissionsFromUser(context, ['ohos.permission.APP_TRACKING_CONSENT']).then((data) => {
        if (data.authResults[0] === 0) {
          hilog.info(0x0000, 'testTag', '%{public}s', 'succeeded in requesting permission');
        } else {
          hilog.info(0x0000, 'testTag', '%{public}s', 'user rejected');
        }
      }).catch((err: BusinessError) => {
        hilog.error(0x0000, 'testTag', '%{public}s', `request permission failed, error: ${err.code} ${err.message}`);
      })
    } catch (err) {
      hilog.error(0x0000, 'testTag', '%{public}s', `catch err->${err.code}, ${err.message}`);
    }
  }

  build() {
    Column() {
      this.actionBar($r('app.string.oaid_title'))
      Column() {
        // Display area for the obtained OAID.
        FormItemText({
          label: $r('app.string.oaid_label'),
          value: $oaid,
          compId: 'hiad_demo_oaid_textArea'
        }).width('90%')

        // 1. Obtain the OAID through a promise.
        this.customButton($r('app.string.oaid_btn_promise'), () => {
          this.getOaidByPromise();
        });

        // 2. Obtain the OAID through an asynchronous callback.
        this.customButton($r('app.string.oaid_btn_asynccallback'), () => {
          this.getOaidByCallback();
        });
      }
      .width('100%')
      .height('80%')
      .justifyContent(FlexAlign.Center)
    }
    .width('100%')
    .height('100%')
  }

  private getOaidByPromise() {
    try {
      // Obtain the OAID.
      identifier.getOAID().then((data: string) => {
        this.oaid = data;
        hilog.info(0x0000, 'testTag', '%{public}s', 'succeeded in getting adsIdentifierInfo by promise');
      }).catch((err: BusinessError) => {
        hilog.error(0x0000, 'testTag', '%{public}s',
          `getAdsIdentifierInfo failed, message: ${(err as BusinessError).message}`);
      })
    } catch (err) {
      hilog.error(0x0000, 'testTag', '%{public}s', `get oaid by callback catch error: ${err.code} ${err.message}`);
    }
  }

  private getOaidByCallback() {
    try {
      identifier.getOAID((err: BusinessError, data: string) => {
        if (err.code) {
          hilog.error(0x0000, 'testTag', '%{public}s', `getAdsIdentifierInfo failed, message: ${err.message}`);
        } else {
          this.oaid = data;
          hilog.info(0x0000, 'testTag', '%{public}s', 'succeeded in getting OaidFromOaidSaAPi by callback');
        }
      });
    } catch (err) {
      hilog.error(0x0000, 'testTag', '%{public}s', `get oaid by callback catch error: ${err.code} ${err.message}`);
    }
  }

  @Builder
  actionBar(title: Resource | string) {
    Row() {
      if (title) {
        Text(title)
          .fontSize(24)
          .fontColor(Color.White)
          .fontWeight(FontWeight.Medium)
          .margin({ left: 16 })
      }
    }
    .width('100%')
    .height('10%')
    .padding({
      top: 16,
      bottom: 16,
      left: 24,
      right: 24
    })
    .backgroundColor(Color.Black)
  }

  @Builder
  customButton(mText: string | Resource, mOnClick: (event?: ClickEvent) => void) {
    Column() {
      Button(mText)
        .backgroundColor($r('app.color.custom_button_background_color'))
        .fontSize(20)
        .fontColor($r('app.color.custom_button_font_color'))
        .fontWeight(FontWeight.Normal)
        .align(Alignment.Center)
        .type(ButtonType.Capsule)
        .width('90%')
        .height(40)
        .margin({ top: 10, bottom: 5 })
        .onClick(mOnClick);
    }
  }
}

@Component
export struct FormItemText {
  @Link @Watch('WatchValueChange') value: string | boolean;
  label: Resource | string = '';
  compId: string = '';

  WatchValueChange() {
    hilog.debug(0x0000, 'testTag', '%{public}s', `WatchValueChange value: ${this.value}`);
  }

  build() {
    Row() {
      Text(this.label)
        .fontSize(20)
        .fontColor($r('app.color.form_item_text_font_color'))
      Text(this.value + '')
        .height(40)
        .borderRadius(20)
        .fontSize(14)
        .backgroundColor($r('app.color.form_item_text_background_color'))
        .textAlign(TextAlign.Center)
        .layoutWeight(1)
        .margin({ left: 5 })
        .copyOption(CopyOptions.InApp)
        .id(this.compId)
    }
    .justifyContent(FlexAlign.Start)
    .alignItems(VerticalAlign.Center)
    .width('100%')
    .height(50)
  }
}
// [End sampleCode]