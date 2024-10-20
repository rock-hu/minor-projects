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

import { NavDestinationItem } from '../viewmodel/NavDestinationItem';
import Constants from '../common/Constants';
import { GlobalBuilderContext } from '../common/GlobalBuilderContext';
import { SearchComponent } from '../feature/searchComponent/view/SearchComponent'
import { SwiperComponent } from '../feature/swiperComponent/view/SwiperComponent'

@Entry
@Component
struct Index {
  private NavDestinationData: NavDestinationItem[] = Constants.NavDestinationData;
  @Provide('NavPathStack') pageInfos: NavPathStack = new NavPathStack()

  @Builder
  PagesMap(name: string) {
    if ((GlobalBuilderContext.getContext().getObject(name)) !== undefined) {
      (GlobalBuilderContext.getContext().getObject(name) as WrappedBuilder<[string, number]>).builder(name, 0)
    }
  }

  aboutToAppear(): void {
    AppStorage.setOrCreate('listData', this.NavDestinationData)
  }

  build() {
    Column() {
      Navigation(this.pageInfos) {

        SearchComponent()
        SwiperComponent()

        List({ space: Constants.LIST_SPACE }) {
          ForEach(this.NavDestinationData, (item: NavDestinationItem) => {
            ListItem() {
              Text(item.detail)
                .width($r('app.string.full_size'))
                .height(Constants.DETAIL_HEIGHT)
                .backgroundColor($r('app.color.start_window_background'))
                .borderRadius(Constants.BORDER_RADIUS)
                .fontSize($r('app.string.search_history_font_size2'))
                .fontColor(Color.Black)
                .fontWeight(Constants.FONT_WEIGHT)
                .textAlign(TextAlign.Center)
                .onClick(() => {
                  this.pageInfos.pushPathByName(item.name, '');
                })
            }
          }, (item: NavDestinationItem) => JSON.stringify(item))
        }
        .scrollBar(BarState.Off)
        .width($r('app.string.common_navigation_width'))
        .height($r('app.string.common_navigation_height'))
        .margin({ top: $r('app.string.main_page_padding') })
      }
      .mode(NavigationMode.Stack)
      .titleMode(NavigationTitleMode.Mini)
      .title(Constants.NavDestinationTitle)
      .navDestination(this.PagesMap)
    }
    .height($r('app.string.full_size'))
    .width($r('app.string.full_size'))
    .backgroundColor($r('app.color.start_background'))
  }
}
