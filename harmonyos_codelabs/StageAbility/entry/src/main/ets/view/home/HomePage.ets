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

import { common } from '@kit.AbilityKit';
import HomeViewModel from '../../viewmodel/HomeViewModel';
import TopBarComponent from './TopBarComponent';
import TabsComponent from './TabsComponent';
import SearchComponent from './SearchComponent';
import MenusComponent from './MenusComponent';
import BannerComponent from './BannerComponent';
import GoodsComponent from './GoodsComponent';
import BannerData from '../../viewmodel/BannerData';
import MenuData from '../../viewmodel/MenuData';
import GoodsData from '../../viewmodel/GoodsData';
import { HomePageStyle, PERCENTAGE_100 } from '../../common/constants/Constants';

let viewModel: HomeViewModel = new HomeViewModel();

@Component
export default struct HomePage {
  @State menus: MenuData[] = [];
  @State tabMenus: Resource[] = [];
  @State bannerList: BannerData[] = [];
  @State goodsList: GoodsData[] = [];

  aboutToAppear() {
    this.goodsList = viewModel.getGoodsList();
    this.bannerList = viewModel.loadBanner();
    this.tabMenus = viewModel.loadTabViewMenu();
    this.menus = viewModel.loadMenus();
  }

  build() {
    Column() {
      Blank()
        .height(HomePageStyle.BLANK_HEIGHT)
      // Logo and QR code.
      TopBarComponent()
        .padding({
          top: HomePageStyle.PADDING_VERTICAL,
          bottom: HomePageStyle.PADDING_VERTICAL,
          left: HomePageStyle.PADDING_HORIZONTAL,
          right: HomePageStyle.PADDING_HORIZONTAL
        })
      SearchComponent()
      TabsComponent({ tabMenus: this.tabMenus })
      BannerComponent({ bannerList: this.bannerList })
      MenusComponent({ menus: this.menus })
      // Goods list view.
      GoodsComponent({
        goodsList: this.goodsList, startPage: (index) => {
          let handler = this.getUIContext().getHostContext() as common.UIAbilityContext;
          viewModel.startDetailsAbility(handler, index);
        }
      })
    }
    .width(PERCENTAGE_100)
    .height(PERCENTAGE_100)
    .backgroundImage($rawfile('index/index_background.png'), ImageRepeat.NoRepeat)
    .backgroundImageSize(ImageSize.Cover)
  }
}