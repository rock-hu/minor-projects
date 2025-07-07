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

import { TabInfo } from '../viewmodel/TabInfo';
import { TabViewModel } from '../viewmodel/TabViewModel';

@Entry
@Component
struct TabsPage {
  @Consume('pageInfos') pageInfo: NavPathStack;
  // [Start Tabs]
  @State currentIndex: number = 0;
  @State hoverIndex: number = 0;
  @State isHover: boolean = false;

  @Builder
  tabBuilder(index: number, name: string, icon: Resource) {
    Column() {
      Row() {
        Image(icon)
          // [StartExclude Tabs]
          .width(48)
          .height(48)
          .borderRadius(8)
          // [EndExclude Tabs]
        if (this.isHover) {
          Text(name)
            .fontColor(this.hoverIndex === index ? '#E6000000' : '#FFFFFF')
            // [StartExclude Tabs]
            .fontSize(18.89)
            .fontWeight(500)
            .lineHeight(25)
            .margin({
              left: 16
            })
            // [EndExclude Tabs]
        }
      }
      .width('100%')
      .height('100%')
      .justifyContent(this.isHover ? FlexAlign.Start : FlexAlign.Center)
      .focusable(false)
      .padding({
        left: this.isHover ? this.hoverIndex === index ? 14 : 12 : 0
      })
    }
    .width(this.hoverIndex === index && this.isHover ? 265 : this.isHover ? 244 : 48)
    .height(64)
    .borderRadius(12)
    .backgroundColor(this.hoverIndex === index && this.isHover ? '#4DFFFFFF' : this.isHover ? '#33F1F3F5' : '00FFFFFFF')
    // Bind hover events and change the state variable when it is judge that the pointer is hovering.
    .onHover((isHover: boolean, event: HoverEvent) => {
      if (isHover) {
        this.hoverIndex = index;
      } else {
        this.hoverIndex = -1;
      }
    })
    .onClick(() => {
      this.currentIndex = index;
    })
  }

  build() {
    SideBarContainer(SideBarContainerType.Embed) {
      Column({ space: 20 }) {
        ForEach(TabViewModel.getTabInfo(), (item: TabInfo) => {
          this.tabBuilder(item.index, item.name, item.icon)
        }, (item: TabInfo, index: number) => index + JSON.stringify(item))
      }
      .height('100%')
      .width(this.isHover ? 324 : 144)
      .justifyContent(this.isHover ? FlexAlign.Start : FlexAlign.Center)
      .backgroundColor(this.isHover ? '#FF212121' : '#FF070707')
      .padding({
        top: this.isHover ? 47 : 0
      })
      // Bind hover events and change the state variable when it is judge that the pointer is hovering.
      .onHover((isHover: boolean, event: HoverEvent) => {
        this.isHover = isHover;
      })
      // [StartExclude Tabs]
      if (this.currentIndex === 0) {
        this.tabContentBuilder(TabViewModel.getTabInfo()[this.currentIndex].name)
      } else if (this.currentIndex === 1) {
        this.tabContentBuilder(TabViewModel.getTabInfo()[this.currentIndex].name)
      } else if (this.currentIndex === 2) {
        this.tabContentBuilder(TabViewModel.getTabInfo()[this.currentIndex].name)
      } else if (this.currentIndex === 3) {
        this.tabContentBuilder(TabViewModel.getTabInfo()[this.currentIndex].name)
      }
      // [EndExclude Tabs]
    }
    .width('100%')
    .height('100%')
    .sideBarWidth(this.isHover ? 324 : 144)
    .showControlButton(false)
    .divider({ strokeWidth: 0, color: '#00000000' })
  }
  // [End Tabs]

  @Builder
  tabContentBuilder(name: string) {
    Row() {
      Text(name)
        .fontSize(50)
        .fontColor(Color.White)
    }
    .justifyContent(FlexAlign.Center)
    .width('100%')
    .height('100%')
    .backgroundColor('#FF070707')
  }
}

@Builder
export function TabsPageBuilder() {
  NavDestination() {
    TabsPage()
  }
  .hideTitleBar(true)
}