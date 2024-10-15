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

import HomePage from '../view/home/HomePage';
import NavViewModel from '../viewmodel/NavViewModel';
import ToolBarData from '../viewmodel/ToolBarData';
import { AppFontSize, AppFontWeight, NavPageStyle, PERCENTAGE_100, ToolBarStyle } from '../common/constants/Constants';

const HOME: number = 0;
let viewModel: NavViewModel = new NavViewModel();

@Entry
@Component
struct NavPage {
  @State toolBarConfigs: ToolBarData[] = []; // bottom navigation data.
  @State navCurrentPosition: number = NavPageStyle.POSITION_INITIAL;
  private pageInfo: NavPathStack = new NavPathStack();

  aboutToAppear() {
    this.toolBarConfigs = viewModel.loadNavigationTab();
    this.pageInfo.pushPath({ name: 'mainPage' });
  }

  build() {
    Navigation(this.pageInfo)
      .hideTitleBar(true)
      .hideNavBar(true)
      .width(PERCENTAGE_100)
      .height(PERCENTAGE_100)
      .navDestination(this.pageMapBuilder)
  }

  @Builder
  pageMapBuilder(name: string) {
    if (name === 'mainPage') {
      NavDestination() {
        Tabs({ barPosition: BarPosition.End, index: $$this.navCurrentPosition }) {
          ForEach(this.toolBarConfigs, (item: ToolBarData, index: number) => {
            TabContent() {
              if (index === HOME) {
                HomePage()
              } else {
                this.holderPage(item?.text)
              }
            }
            .tabBar(this.tabBarBuilder(item?.text, index))
          }, (item: ToolBarData) => JSON.stringify(item))
        }
        .scrollable(false)
      }
    }
  }

  @Builder
  tabBarBuilder(name: ResourceStr, index: number) {
    Column() {
      Image(this.navCurrentPosition === index ? this.toolBarConfigs[index]?.icon_after : this.toolBarConfigs[index]?.icon)
        .height(ToolBarStyle.IMAGE_SIZE)
        .margin({ bottom: ToolBarStyle.MARGIN_BOTTOM })
        .objectFit(ImageFit.Contain)
      Text(name)
        .fontColor(this.navCurrentPosition === index ? $r('app.color.index_navigation_selected_text') :
          $r('app.color.text'))
        .fontWeight(AppFontWeight.BOLDER)
        .fontSize(AppFontSize.SMALLER)
        .textAlign(TextAlign.Center)
    }
  }

  @Builder
  holderPage(text: Resource) {
    Column() {
      Text(text)
        .fontSize(AppFontSize.LARGER)
        .backgroundColor(Color.White)
        .height(PERCENTAGE_100)
        .width(PERCENTAGE_100)
        .textAlign(TextAlign.Center)
    }
    .justifyContent(FlexAlign.Center)
  }
}