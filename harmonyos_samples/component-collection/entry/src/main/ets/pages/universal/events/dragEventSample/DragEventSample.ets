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

import { IntroductionTitle } from '../../../../common/IntroductionTitle';
import { GridItemDrag } from './component/GridItemDrag';
import { HyperlinkDrag } from './component/HyperlinkDrag';
import { ImageDrag } from './component/ImageDrag';
import { ListItemDrag } from './component/ListItemDrag';
import { TextDrag } from './component/TextDrag';
import { VideoDrag } from './component/VideoDrag';
import { router } from '@kit.ArkUI';

@Extend(Row)
function rowStyle() {
  .width('100%')
  .padding(16)
  .borderRadius(24)
  .backgroundColor(Color.White)
}

@Extend(Column)
function columnStyle() {
  .width('100%')
  .padding(16)
  .borderRadius(24)
  .backgroundColor(Color.White)
}

@Entry
@Component
struct DragEventSample {
  // Receivable
  @State isAreaValid: boolean = false;
  @State backgroundBlur: boolean = false;
  @State hideLeftIcon: boolean = true;
  @State hideRightIcon: boolean = true;
  @State showOkBadge: boolean = false;
  @State showCancelBadge: boolean = false;
  @State leftIconWidth: number = 64;
  @State rightIconWidth: number = 64;

  // Customize the content displayed during dragging
  @Builder
  pixelMapBuilder() {
    Stack({ alignContent: Alignment.TopEnd }) {
      Image($r('app.media.icon'))
        .width(64)
        .aspectRatio(1)
        .margin(16)
      if (this.isAreaValid) {
        Image($r('app.media.ic_ok'))
          .width(32)
          .aspectRatio(1)
          .fillColor(Color.Green)
      } else {
        Image($r('app.media.ic_cancel'))
          .width(32)
          .aspectRatio(1)
          .fillColor(Color.Red)
      }
    }
  }

  @Builder
  menuBuilder() {
    Column() {
      Text($r('app.string.drag_drop_action_tips'))
        .fontSize(16)
    }
    .alignItems(HorizontalAlign.Center)
    .justifyContent(FlexAlign.Center)
    .padding(16)
    .borderRadius(12)
  }

  build() {
    Column() {
      Column() {
        Row() {
          Image($r('app.media.ic_back'))
            .width(20)
            .height(20)
            .margin({ left: 26 })
            .objectFit(ImageFit.Contain)
            .onClick(() => {
              this.getUIContext().getRouter().back()
            })
            .id('backBtn')
          Text($r('app.string.drag_drop_event'))
            .fontSize(20)
            .layoutWeight(1)
            .margin({ left: 16 })
            .align(Alignment.Start)
          Blank()
        }
        .height(56)
        .width('100%')
      }

      Scroll() {
        Column() {
          IntroductionTitle({ introduction: $r('app.string.drag_drop_action_tips') })
          Stack() {
            Row() {
              // Green acceptable area on the left
              Row() {
                Stack({ alignContent: Alignment.TopEnd }) {
                  Image($r('app.media.icon'))
                    .width(this.leftIconWidth)
                    .aspectRatio(1)
                    .margin(this.leftIconWidth / 4)
                    .visibility(this.hideLeftIcon ? Visibility.None : Visibility.Visible)
                    .animation({
                      onFinish: () => {
                        this.showOkBadge = false;
                      }
                    })
                  if (this.showOkBadge) {
                    Image($r('app.media.ic_ok'))
                      .width(this.leftIconWidth / 2)
                      .aspectRatio(1)
                      .fillColor(Color.Green)
                      .animation({})
                  }
                }
              }
              .id('leftArea')
              .width('45%')
              .height(200)
              .backgroundColor($r('app.color.background_green'))
              .justifyContent(FlexAlign.Center)
              .onDragEnter(() => {
                this.isAreaValid = true;
              })
              .onDragMove(() => {
                this.isAreaValid = true;
              })
              .onDragLeave(() => {
                this.isAreaValid = false;
              })
              .onDrop(() => {
                this.hideLeftIcon = false;
                this.showOkBadge = true;
                this.leftIconWidth = 48;
              })

              Blank()

              // The red unacceptable area on the right
              Row() {
                Stack({ alignContent: Alignment.TopEnd }) {
                  Image($r('app.media.icon'))
                    .width(this.rightIconWidth)
                    .aspectRatio(1)
                    .margin(this.rightIconWidth / 4)
                    .visibility(this.hideRightIcon ? Visibility.None : Visibility.Visible)
                    .animation({
                      onFinish: () => {
                        this.hideRightIcon = true;
                        this.showCancelBadge = false;
                        this.rightIconWidth = 64;
                      }
                    })
                  if (this.showCancelBadge) {
                    Image($r('app.media.ic_cancel'))
                      .width(this.rightIconWidth / 2)
                      .aspectRatio(1)
                      .fillColor('#FF0000')
                      .animation({})
                  }
                }
              }
              .id('rightArea')
              .width('45%')
              .height(200)
              .backgroundColor($r('app.color.background_pink'))
              .justifyContent(FlexAlign.Center)
              .onDragEnter(() => {
                this.isAreaValid = false;
              })
              .onDragMove(() => {
                this.isAreaValid = false;
              })
              .onDrop(() => {
                this.hideRightIcon = false;
                this.showCancelBadge = true;
                this.rightIconWidth = 72;
              })
            }
            .rowStyle()

            // Fuzzy Layer
            Row() {
              Blank()

              // Middle drag icon
              Column() {
                if (this.hideLeftIcon) {
                  Row() {
                    Image($r('app.media.icon'))
                      .id('icon')
                      .width(48)
                      .aspectRatio(1)
                      .draggable(false)
                  }
                  .bindContextMenu(this.menuBuilder(), ResponseType.LongPress)
                  .onDragStart(() => {
                    this.backgroundBlur = false;
                    // Close menu while dragging
                    this.getUIContext().getContextMenuController().close();
                    return this.pixelMapBuilder;
                  })
                  .onTouch((event: TouchEvent) => {
                    if (event.type === TouchType.Down) {
                      this.backgroundBlur = true;
                    }
                  })
                }
              }
              .width('10%')
              .height(200)
              .justifyContent(FlexAlign.Center)

              Blank()
            }
            .width('100%')
            .padding(16)
            .borderRadius(24)
            .backgroundBlurStyle(this.backgroundBlur ? BlurStyle.BACKGROUND_THIN : 0)
            .onClick(() => {
              this.backgroundBlur = false;
            })
          }

          IntroductionTitle({ introduction: $r('app.string.component_drag_and_drop') })
          Column() {
            IntroductionTitle({ introduction: $r('app.string.griditem') })
            GridItemDrag()

            IntroductionTitle({ introduction: $r('app.string.listitem') })
            ListItemDrag()

            IntroductionTitle({ introduction: $r('app.string.hyperlink') })
            HyperlinkDrag()

            IntroductionTitle({ introduction: $r('app.string.text') })
            TextDrag()

            IntroductionTitle({ introduction: $r('app.string.image') })
            ImageDrag()

            IntroductionTitle({ introduction: $r('app.string.video') })
            VideoDrag()
          }
          .id('column')
          .columnStyle()
          .margin({ bottom: 80 })
        }
      }
      .padding({ left: 12, right: 12 })
    }
    .backgroundColor($r('app.color.background_shallow_grey'))
  }
}