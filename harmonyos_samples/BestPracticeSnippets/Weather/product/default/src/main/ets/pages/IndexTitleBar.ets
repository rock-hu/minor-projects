/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { router } from '@kit.ArkUI';
import UpdateTimeDialog from './UpdateTimeDialog';
import { getMenuInfo, MenuData, Style } from 'common';

@Component
export default struct IndexTitleBar {
  @State popupState: boolean = false;
  @StorageProp('curBp') @Watch('curBpChange') curBp: string = 'sm';
  @Link showSideBar: boolean;
  @StorageLink('titleText') titleText: string[] = [];
  @StorageLink('swiperIndex') swiperIndex: number = 0;
  showDialog: boolean = false;
  dialogController?: CustomDialogController;

  @Builder
  popupBuilder() {
    Row() {
      Column() {
        ForEach(getMenuInfo(), (item: MenuData, index: number) => {
          if (index > 0) {
            Divider()
              .width('100%')
              .color('#AFAFAF')
              .margin({ top: 10, bottom: 10 })
          }
          Text(item.title)
            .id('menu' + index)
            .fontSize(16)
            .fontColor('#000000')
            .fontWeight(FontWeight.Regular)
            .width('100%')
            .onClick(() => {
              if (item.url !== '') {
                this.popupState = false;
                if (this.showSideBar) {
                  this.showSideBar = false;
                }
                this.getUIContext().getRouter().pushUrl({ url: item.url });
              } else {
                if (this.dialogController === undefined) {
                  this.dialogController = new CustomDialogController({
                    builder: UpdateTimeDialog({ handleCancelDialog: this.cancelDialog }),
                    autoCancel: false,
                    customStyle: true
                  });
                }
                this.showDialog = true;
                this.popupState = false;
                this.dialogController.open();
              }
            })
        }, (item: MenuData, index: number) => JSON.stringify(item) + index)
      }
      .width(160)
      .padding(15)
      .backgroundColor(Color.White)
      .borderRadius(Style.NORMAL_RADIUS)
    }
    .padding({ right: 12 })
  }

  build() {
    Row() {
      if (!this.showSideBar && this.curBp === 'lg') {
        Image($r('app.media.icon_subarea'))
          .id('iconSubarea')
          .size({ width: 24, height: 24 })
          .objectFit(ImageFit.Contain)
          .onClick(() => {
            this.showSideBar = !this.showSideBar;
          })
      }
      Text(this.titleText[this.swiperIndex])
        .fontSize(20)
        .fontColor(Color.White)
        .textAlign(TextAlign.Center)
        .margin({ left: (!this.showSideBar && this.curBp === 'lg') ? 0 : 25 })
        .layoutWeight(1)

      Image($r('app.media.icon_more'))
        .id('menuPopup')
        .size({ width: 24, height: 24 })
        .objectFit(ImageFit.Fill)
        .bindPopup(this.popupState, {
          builder: this.popupBuilder,
          placement: Placement.BottomLeft,
          maskColor: 0x33000000,
          popupColor: '#00000000',
          enableArrow: false,
          onStateChange: (e) => {
            if (!e.isVisible) {
              this.popupState = false;
            }
          }
        })
        .onClick(() => {
          this.popupState = !this.popupState;
        })
    }
    .width('100%')
    .height('100%')
    .padding({ left: Style.NORMAL_PADDING, right: Style.NORMAL_PADDING })
  }

  cancelDialog = () => {
    this.dialogController?.close();
    this.showDialog = false;
  }

  curBpChange() {
    if (this.showDialog) {
      this.dialogController?.close();
      this.dialogController = new CustomDialogController({
        builder: UpdateTimeDialog({ handleCancelDialog: this.cancelDialog }),
        autoCancel: false,
        customStyle: true
      });
      this.dialogController.open();
    }
  }
}