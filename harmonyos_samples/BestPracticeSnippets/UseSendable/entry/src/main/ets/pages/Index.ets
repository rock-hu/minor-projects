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

import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { InterThreadCommunication1 } from './InterThreadCommunication1';
import { InterThreadCommunication2 } from './InterThreadCommunication2';
import { InterThreadCommunication3 } from './InterThreadCommunication3';

const DOMAIN = 0x0000;
const TAG = 'Index';

@Entry
@Component
struct Index {
  private pageStack: NavPathStack = new NavPathStack();

  @Styles
  buttonStyles() {
    .width('100%')
    .margin({
      bottom: 12
    })
  }

  @Builder
  pageMap(pageName: string): void {
    if (pageName === 'InterThreadCommunication1') {
      InterThreadCommunication1()
    } else if (pageName === 'InterThreadCommunication2') {
      InterThreadCommunication2()
    } else if (pageName === 'InterThreadCommunication3') {
      InterThreadCommunication3()
    }
  }

  build() {
    Navigation(this.pageStack) {
      Column() {
        Button('跨语言多线程通信（C++与ArkTS）')
          .buttonStyles()
          .onClick(() => {
            this.pageStack.pushDestinationByName('InterThreadCommunication1', null, true)
              .catch((err: BusinessError) => {
                hilog.error(DOMAIN, TAG, `pushDestinationByName failed. Code: ${err.code}, message: ${err.message}`);
              });
          })

        Button('线程间模块共享（单例模式）')
          .width('100%')
          .buttonStyles()
          .onClick(() => {
            this.pageStack.pushDestinationByName('InterThreadCommunication2', null, true)
              .catch((err: BusinessError) => {
                hilog.error(DOMAIN, TAG, `pushDestinationByName failed. Code: ${err.code}, message: ${err.message}`);
              });
          })

        Button('线程间模块共享（单例模式）')
          .width('100%')
          .buttonStyles()
          .onClick(() => {
            this.pageStack.pushDestinationByName('InterThreadCommunication3', null, true)
              .catch((err: BusinessError) => {
                hilog.error(DOMAIN, TAG, `pushDestinationByName failed. Code: ${err.code}, message: ${err.message}`);
              });
          })
      }
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.End)
      .padding({
        right: 24,
        left: 24
      })
    }
    .title($r('app.string.home_title'))
    .navDestination(this.pageMap)
    .hideToolBar(true)
  }
}