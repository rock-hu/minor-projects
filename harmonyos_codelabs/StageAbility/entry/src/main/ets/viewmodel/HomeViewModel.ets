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

import { hilog } from '@kit.PerformanceAnalysisKit';
import { common } from '@kit.AbilityKit';
import { Want } from '@kit.AbilityKit';
import DataModel from '../model/DataModel';
import GoodsData from './GoodsData';
import BannerData from './BannerData';
import MenuData from './MenuData';
import { DETAILS_ABILITY_NAME } from '../common/constants/Constants';

const TAG = 'HomeViewModel';
const HOME_PAGE_DOMAIN = 0x00004;

export default class HomeViewModel {
  public getGoodsList(): GoodsData[] {
    return DataModel.GOOD_LIST;
  }

  public loadTabViewMenu(): Resource[] {
    return DataModel.TAB_VIEW_MENU;
  }

  public loadMenus(): MenuData[] {
    return DataModel.HOME_MENU;
  }

  public loadBanner(): BannerData[] {
    return DataModel.BANNER;
  }

  public startDetailsAbility(context: common.UIAbilityContext, index: number): void {
    const want: Want = {
      bundleName: context.applicationInfo.name,
      abilityName: DETAILS_ABILITY_NAME,
      parameters: {
        position: index
      }
    };
    try {
      context.startAbility(want);
    } catch (error) {
      hilog.error(HOME_PAGE_DOMAIN, TAG, '%{public}s', error);
    }
  }
}