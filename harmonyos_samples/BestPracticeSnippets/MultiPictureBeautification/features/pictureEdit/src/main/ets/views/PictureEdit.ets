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

import { BaseConstants, BreakpointConstants } from '@ohos/commons/';
import PictureEditConstants from '../constants/PictureEditConstants';
import { deviceInfo } from '@kit.BasicServicesKit';

interface ToolsAndName {
  pic: Resource
  pic_name: string
}

interface PicAndName {
  pic: Resource
  pic_name: string
  color_filter: number[]
}

@Component
export struct PictureEdit {
  @StorageLink('currentBreakpoint') currentBp: string = BreakpointConstants.BREAKPOINT_MD;
  @State outSetValueOne: number = 40;
  @State windowDirection: boolean = true;
  @State pictureWidth: string = BaseConstants.FULL_WIDTH;
  @Consume('pageInfos') pageInfos: NavPathStack;

  isColumnLayout(bp: string, direction: Boolean): Boolean {
    if (bp === BreakpointConstants.BREAKPOINT_SM) {
      this.pictureWidth = PictureEditConstants.PICTURE_WIDTH_SM;
      return true;
    }
    if (bp === BreakpointConstants.BREAKPOINT_LG) {
      this.pictureWidth = PictureEditConstants.PICTURE_WIDTH_LG;
      return false;
    }
    if (direction) {
      this.pictureWidth = PictureEditConstants.PICTURE_WIDTH_MD_ROW;
    } else {
      this.pictureWidth = PictureEditConstants.PICTURE_WIDTH_MD_COLUMN;
    }
    return !direction;
  }

  @Builder
  topBar() {
    Row() {
      Image($r('app.media.ic_public_back'))
        .width(BaseConstants.DEFAULT_ICON_SIZE)
        .height(BaseConstants.DEFAULT_ICON_SIZE)
        .onClick(() => {
          this.pageInfos.pop();
        })
        .margin({
          left: $r('app.float.back_image_left'),
          right: $r('app.float.back_image_right')
        })

      Text(PictureEditConstants.Edit_Text)
        .fontSize(BaseConstants.FONT_SIZE_TWENTY)
        .fontColor(Color.White)
      Blank()
      if (this.currentBp === BreakpointConstants.BREAKPOINT_MD) {
        Image(this.windowDirection ? $r('app.media.change2') : $r('app.media.change'))
          .width(BaseConstants.DEFAULT_ICON_SIZE)
          .height(BaseConstants.DEFAULT_ICON_SIZE)
          .margin({ right: $r('app.float.image_right') })
          .onClick(() => {
            this.windowDirection = !this.windowDirection;
          })
      }
      Image($r('app.media.ic_add_2'))
        .width(BaseConstants.DEFAULT_ICON_SIZE)
        .height(BaseConstants.DEFAULT_ICON_SIZE)
        .margin({ right: $r('app.float.image_right') })
      Image($r('app.media.ic_add_3'))
        .width(BaseConstants.DEFAULT_ICON_SIZE)
        .height(BaseConstants.DEFAULT_ICON_SIZE)
        .margin({ right: $r('app.float.image_right') })

      Image($r('app.media.ic_add_4'))
        .width(BaseConstants.DEFAULT_ICON_SIZE)
        .height(BaseConstants.DEFAULT_ICON_SIZE)
        .margin({ right: $r('app.float.image_right') })

    }
    .alignItems(VerticalAlign.Center)
    .height($r('app.float.row_height'))
    .width(BaseConstants.FULL_WIDTH)
    .backgroundColor(Color.Black)
  }

  @Builder
  centerPicture() {
    Flex({ justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
      Image($r("app.media.photo"))
        .interpolation(ImageInterpolation.High)
        .objectFit(ImageFit.Cover)
        .autoResize(true)
        .height(PictureEditConstants.PICTURE_HEIGHT)
        .width(this.pictureWidth)
    }
  }

  @Builder
  optionRegion() {
    Flex({
      direction: this.isColumnLayout(this.currentBp, this.windowDirection) ? FlexDirection.Column : FlexDirection.Row
    }) {
      Flex() {
        this.sliderBar()
      }
      .flexBasis(PictureEditConstants.SLIDER_FLEX_BASIS)

      this.filterWindows()
      Flex({ justifyContent: FlexAlign.Center }) {
        this.bottomBar()
      }
      .flexBasis(PictureEditConstants.BAR_FLEX_BASIS)
    }
    .height(this.isColumnLayout(this.currentBp, this.windowDirection) ? BaseConstants.FULL_HEIGHT :
    PictureEditConstants.PICTURE_HALF_HEIGHT)
    .padding($r('app.float.row_padding'))
  }

  @Builder
  sliderBar() {
    Column() {
      Slider({
        value: this.outSetValueOne,
        min: 0,
        max: 100,
        style: SliderStyle.OutSet,
        direction: this.isColumnLayout(this.currentBp, this.windowDirection) ? Axis.Horizontal : Axis.Vertical,
        reverse: this.isColumnLayout(this.currentBp, this.windowDirection) ? false : true
      })
        .blockColor(Color.White)
        .selectedColor(Color.White)
        .trackColor($r('app.color.track_color'))
        .showTips(true)
        .onChange((value: number) => {
          this.outSetValueOne = value;
        })
    }
  }

  @Builder
  filterWindows() {
    List() {
      ForEach(PictureEditConstants.filterAndName, (item: PicAndName) => {
        ListItem() {
          Stack() {
            Image(item.pic)
              .height($r('app.float.image_height'))
              .aspectRatio(1)
              .autoResize(true)
              .colorFilter(item.color_filter)
              .clip(true)
              .borderRadius(PictureEditConstants.IMAGE_BORDER_RADIUS)
            Text(item.pic_name)
              .fontSize(BaseConstants.FONT_SIZE_TEN)
              .fontColor($r('app.color.text_color'))
              .margin({ left: $r('app.float.text_margin'), bottom: $r('app.float.text_margin') })
          }
          .alignContent(Alignment.BottomStart)
        }
      },(item: PicAndName, index: number) => index + JSON.stringify(item))
    }
    .listDirection(this.isColumnLayout(this.currentBp, this.windowDirection) ? Axis.Horizontal : Axis.Vertical)
  }

  @Builder
  bottomBar() {
    Flex({
      justifyContent: this.isColumnLayout(this.currentBp, this.windowDirection) ? FlexAlign.SpaceAround :
      FlexAlign.SpaceAround,
      alignItems: ItemAlign.Center,
      direction: this.isColumnLayout(this.currentBp, this.windowDirection) ? FlexDirection.Row : FlexDirection.Column,
    }) {

      ForEach(PictureEditConstants.toolsAndName, (item: ToolsAndName) => {
        Column() {
          Image(item.pic)
            .height(BaseConstants.DEFAULT_ICON_SIZE)
            .autoResize(true)
            .aspectRatio(1)
            .margin({ bottom: $r('app.float.image_margin') })
          Text(item.pic_name)
            .fontSize(BaseConstants.FONT_SIZE_TEN)
            .fontColor($r('app.color.text_color'))
        }
      }, (item: ToolsAndName) => item.pic_name
      )
    }
  }

  build() {
    NavDestination(){
      Flex({ direction: FlexDirection.Column }) {
        this.topBar()
        Flex({
          direction: this.isColumnLayout(this.currentBp, this.windowDirection) ? FlexDirection.Column : FlexDirection.Row
        }) {
          Flex() {
            this.centerPicture()
          }
          .flexShrink(1)

          Flex({ justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
            this.optionRegion()
          }
          .flexBasis(PictureEditConstants.OPTION_FLEX_BASIS)
          .height(BaseConstants.FULL_HEIGHT)
        }
      }
      .backgroundColor(Color.Black)
      .padding({
        top: deviceInfo.deviceType === BaseConstants.DEVICE_2IN1 ? $r('app.float.zero') :
        $r('app.float.device_padding_top'),
        bottom: deviceInfo.deviceType !== BaseConstants.DEVICE_2IN1 ?
        $r('app.float.tab_content_pb') : $r('app.float.zero')
      })
    }
    .hideTitleBar(true)

  }
}