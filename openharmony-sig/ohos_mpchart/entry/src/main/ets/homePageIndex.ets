/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import ContentItem from './pageconfig/ContentItem';
import ContentItemData from './pageconfig/ContentItemData';
import router from '@ohos.router';
import title, { ChartTitleModel } from './title/index';

@Entry
@ComponentV2
struct homePageIndex {
  @Local pageItemArr: Array<ContentItemData> = ContentItemData.getData();
  private menuItemArr: Array<string> = ['View on GitHub', 'Problem Report', 'Developer Website'];
  private title: string = 'mpchart Example'
  scroller: Scroller = new Scroller()
  @Local model: ChartTitleModel = new ChartTitleModel()

  build() {
    Column() {
      title({ model: this.model })
      Scroll(this.scroller) {
        Column() {
          ForEach(this.pageItemArr, (item: ContentItem) => {
            if (item.getIsSection()) {
              ItemTitle({ itemData: item })
            } else {
              Flex() {
                Item({ itemData: item });
              }.onClick((event: ClickEvent) => {
                router.pushUrl({
                  url: item.getPagePath(),
                });
              })

            }
          }, (item: ContentItem) => (item.getName() + "").toString())

        }.width('100%').padding({ bottom: 80 })
      }.height('100%')
      .scrollable(ScrollDirection.Vertical)
      .scrollBarWidth(0)
      .backgroundColor(Color.White)
    }
  }

  public aboutToAppear() {
    this.model.menuItemArr = this.menuItemArr
    this.model.title = this.title
    this.model.isShowMenu = false
    this.model.setCanBack(false);
  }

  @Monitor("model")
  menuCallback() {

  }
}

@ComponentV2
struct ItemTitle {
  @Param itemData: ContentItem = new ContentItem('title');
  @Param backGroundTitle: number = Color.White

  build() {
    Column() {
      Text(this.itemData.getName()).fontSize(21).padding(16).fontColor(Color.Gray)
      Blank().width('100%').height(1).backgroundColor(0xdddddd)
    }.backgroundColor(this.backGroundTitle)
  }
}

@ComponentV2
struct Item {
  @Param itemData: ContentItem = new ContentItem('title');
  @Local backGround: BackGroundInfo = new BackGroundInfo(Color.White);

  build() {
    Column() {
      Text(this.itemData.getName())
        .fontSize(16)
        .fontColor(Color.Black)
        .margin({ left: 4 })
        .padding(8)
        .alignSelf(ItemAlign.Start)
        .width('100%')
      Text(this.itemData.getDesc())
        .fontSize(12)
        .fontColor(Color.Gray)
        .margin({ left: 4 })
        .padding({ top: 0, right: 8, bottom: 8, left: 8 })
        .alignSelf(ItemAlign.Start)
        .width('100%')
      Blank().width('100%').height(1).backgroundColor(0xdddddd)
    }.onTouch((event: TouchEvent) => {
      if (event.type == TouchType.Down) {
        this.backGround = new BackGroundInfo(0xdcdcdc)
      }
      if (event.type == TouchType.Up) {
        this.backGround = new BackGroundInfo(Color.White)
      }
    }).backgroundColor(this.backGround.backGround)
  }
}

@ObservedV2
class BackGroundInfo {
  @Trace backGround: number = Color.White;

  constructor(backGround: number) {
    this.backGround = backGround;
  }
}




