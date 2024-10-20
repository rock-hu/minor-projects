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

export default class Constants {
  /**
   * NavDestinationTitle.
   */
  static readonly NavDestinationTitle: string = '动效合集';

  /**
   * FontWeight.borderRadius
   */
  static readonly FONT_WEIGHT: number = 500;

  /**
   * BorderRadius.
   */
  static readonly BORDER_RADIUS: number = 24;

  /**
   * Detail height.
   */
  static readonly DETAIL_HEIGHT: number = 72;

  /**
   * List space.
   */
  static readonly LIST_SPACE: number = 12;

  /**
   * NavDestinationItem.
   */
  static readonly NavDestinationData: NavDestinationItem[] = [
    {
      name: 'PageTurningAnimation', detail: '翻页动效'
    },
    {
      name: 'PageExpandTitle', detail: '标题下拉缩放'
    },
    {
      name: 'PageStatusBarChange', detail: '状态栏显隐切换'
    },
    {
      name: 'PageListSlideToHistory', detail: '长列表滑动到指定列表项动效实现'
    },
    {
      name: 'PageWaterRipples', detail: '听歌识曲水波纹特效'
    },
    {
      name: 'PageWaterMarqueeView', detail: '跑马灯'
    },
    {
      name: 'SwiperSmoothVariation', detail: 'Swiper高度变化效果'
    },
    {
      name: 'SideBarAnimation', detail: 'SideBarContainer侧边栏淡入淡出动效'
    },
    {
      name: 'PageVibrateEffect', detail: '使用弹簧曲线实现抖动动画及手机振动效果'
    },
    {
      name: 'PagePaintComponent', detail: '使用绘制组件实现自定义进度动画'
    },
    {
      name: 'DigitalScrollExample', detail: '数字滚动动效'
    },
    {
      name: 'PageCardsSwiper', detail: '自定义Swiper卡片预览效果'
    },
    {
      name: 'VotingComponent', detail: '投票动效'
    },
    {
      name: 'VoiceRecordDynamic', detail: '语音录制和声音动效实现'
    }
  ];
}
