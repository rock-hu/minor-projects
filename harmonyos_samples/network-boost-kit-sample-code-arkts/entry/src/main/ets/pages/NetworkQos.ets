/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import { hilog } from '@kit.PerformanceAnalysisKit';
import { netQuality } from '@kit.NetworkBoostKit';
import { BusinessError } from '@kit.BasicServicesKit';

@Entry
@Component
struct requestContent {
  @State showToken: string = '';
  @State content: string = '';

  build() {
    Column({ space: 5 }) {
      Row() {
        Text('NetworkQos')
          .fontSize(30)
          .fontWeight(500)
          .fontColor('#FF100F0F')
          .height(30)
          .width(220)
          .margin({ top: '11%' })
      }
      .height(40)
      .width(124)
      .margin({ top: 32 })

      Divider().color('gray').strokeWidth(1)
      Column({ space: 20 }) {
        Button('订阅')
          .width('100%')
          .height(40)
          .margin({ top: '11%' })
          .fontColor('#FFFFFFFF')
          .fontSize(24)
          .fontWeight(500)
          .borderRadius(20)
          .onClick(this.getNetworkQosOn)

        Button('去订阅')
          .width('100%')
          .height(40)
          .margin({ top: '11%' })
          .fontColor('#FFFFFFFF')
          .fontSize(24)
          .fontWeight(500)
          .borderRadius(20)
          .onClick(this.getNetworkQosOff)

        Text('结果')
          .width('100%')
          .height(40)
          .margin({ top: '11%' })
          .fontColor('#000000')
          .fontSize(24)
          .fontWeight(500)
        TextArea({
          text: this.content
        })
          .width('100%')
          .height(260)
          .margin({ top: '11%' })
          .backgroundColor('0x317AFF')
          .fontColor('#FFFFFFFF')
          .fontSize(24)
          .fontWeight(500)
          .borderRadius(10)
      }
      .height('100%')
      .width('100%')
    }
    .height('100%')
    .width('100%')
    .padding({ left: '16vp', right: '16vp' })
    .alignItems(HorizontalAlign.Start)
  }

  private getNetworkQosOn = async () => {
    try {
      netQuality.on('netQosChange', (list: Array<netQuality.NetworkQos>) => {
        hilog.info(0x0000, 'testTag', 'on receive netQosChange event');
        if (list.length > 0) {
          list.forEach((qos) => {
            this.content = `callback info + ${JSON.stringify(qos)}`;
            hilog.info(0x0000, 'testTag', 'on receive netQosChange event:' + `callback info + ${JSON.stringify(qos)}`);
          });
        }
      });
      hilog.info(0x0000, 'testTag', 'on--success');
      this.content = `on--success`;
    } catch (err) {
      let e: BusinessError = err as BusinessError;
      this.content = `on--err` + JSON.stringify(err);
      hilog.error(0x0000, 'testTag', 'on netQosChange error: %{public}d %{public}s', e.code, e.message);
    }
  }

  private getNetworkQosOff = async () => {
    try {
      netQuality.off('netQosChange');
      hilog.info(0x0000, 'testTag', 'off--success');
      this.content = `off--success`;
    } catch (err) {
      let e: BusinessError = err as BusinessError;
      this.content = `off--err` + JSON.stringify(err);
      hilog.error(0x0000, 'testTag', 'off netQosChange error: %{public}d %{public}s', e.code, e.message);
    }
  }
}