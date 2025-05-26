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

import { BaseConstants } from '@ohos/commons';
import { AlbumViewConstants } from '../constants/AlbumViewConstants';

@Component
export struct SideColumn {
  build() {
    Flex({ direction: FlexDirection.Column }) {
      Row() {
        Button({ type: ButtonType.Circle, stateEffect: false }) {
          Image($r('app.media.switch'))
            .width(BaseConstants.DEFAULT_ICON_SIZE)
            .height(BaseConstants.DEFAULT_ICON_SIZE)
        }
        .backgroundColor(Color.White)
        .margin({ right: $r('app.float.side_button_right') })
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.side_button_height'))
      .padding({
        left: $r('app.float.side_button_padding_left'),
        right: $r('app.float.side_button_padding_right'),
        top: $r('app.float.side_button_padding_top'),
        bottom: $r('app.float.side_button_padding_bottom')
      })

      Row() {
        Stack({ alignContent: Alignment.Start }) {
          TextInput({ placeholder: AlbumViewConstants.SideConstants[0] })
            .type(InputType.Normal)
            .enableKeyboardOnFocus(false)

          Image($r('app.media.search2'))
            .aspectRatio(1)
            .autoResize(true)
            .padding({
              left: $r('app.float.search_left'),
              top: $r('app.float.search_top'),
              bottom: $r('app.float.search_bottom')
            })
        }.padding({ left: $r('app.float.search_left') })
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.search_height'))
      .padding({
        right: $r('app.float.search_stack_right'),
        top: $r('app.float.search_stack_top')
      })

      Row() {
        Image($r('app.media.public_ic_public_folder_photo'))
          .height(BaseConstants.DEFAULT_ICON_SIZE)
          .width(BaseConstants.DEFAULT_ICON_SIZE)
          .margin({
            left: $r('app.float.margin_eight'),
            right: $r('app.float.margin_eight')
          })
        Text(AlbumViewConstants.SideConstants[1])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Image($r('app.media.public_ic_public_folder_album'))
          .height(BaseConstants.DEFAULT_ICON_SIZE)
          .width(BaseConstants.DEFAULT_ICON_SIZE)
          .margin({
            left: $r('app.float.margin_eight'),
            right: $r('app.float.margin_eight')
          })
        Text(AlbumViewConstants.SideConstants[1])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Image($r('app.media.ic_public_arrow_right'))
          .height(BaseConstants.DEFAULT_ICON_SIZE)
          .width($r('app.float.image_width'))
          .rotate({
            z: 1,
            centerX: '50%',
            centerY: '50%',
            angle: 90
          })
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[2])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
          .fontColor('#0A59F7')
        Blank()
        Text(AlbumViewConstants.SideConstants[4])
          .fontSize(BaseConstants.FONT_SIZE_FOURTEEN)
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[3])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Text(AlbumViewConstants.SideConstants[4])
          .fontSize(BaseConstants.FONT_SIZE_FOURTEEN)
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[5])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Text(AlbumViewConstants.SideConstants[4])
          .fontSize(BaseConstants.FONT_SIZE_FOURTEEN)
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[6])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Text(AlbumViewConstants.SideConstants[4])
          .fontSize(BaseConstants.FONT_SIZE_FOURTEEN)
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[7])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Text(AlbumViewConstants.SideConstants[4])
          .fontSize(BaseConstants.FONT_SIZE_FOURTEEN)
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Divider()
      Row() {
        Text(AlbumViewConstants.SideConstants[8])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Image($r('app.media.ic_public_arrow_right'))
          .height(BaseConstants.DEFAULT_ICON_SIZE)
          .width($r('app.float.image_width'))
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[9])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Image($r('app.media.ic_public_arrow_right'))
          .height(BaseConstants.DEFAULT_ICON_SIZE)
          .width($r('app.float.image_width'))
          .rotate({
            z: 1,
            centerX: '50%',
            centerY: '50%',
            angle: 90
          })
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[10])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Text(AlbumViewConstants.SideConstants[4])
          .fontSize(BaseConstants.FONT_SIZE_FOURTEEN)
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left2'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[11])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Text(AlbumViewConstants.SideConstants[4])
          .fontSize(BaseConstants.FONT_SIZE_FOURTEEN)
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left2'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[12])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Text(AlbumViewConstants.SideConstants[4])
          .fontSize(BaseConstants.FONT_SIZE_FOURTEEN)
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left2'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })

      Row() {
        Text(AlbumViewConstants.SideConstants[13])
          .fontSize(BaseConstants.FONT_SIZE_SIXTEEN)
        Blank()
        Image($r('app.media.ic_public_arrow_right'))
          .height(BaseConstants.DEFAULT_ICON_SIZE)
          .width($r('app.float.image_width'))
      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.row_height'))
      .padding({
        left: $r('app.float.row_padding_left'),
        right: $r('app.float.row_padding_right'),
        top: $r('app.float.row_padding_top')
      })
    }
  }
}
