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

import { PicAndName, ToolsAndName } from '../viewmodel/AdaptiveViewModel';

export default class PictureEditConstants {
  /**
   * Picture size.
   */
  static PICTURE_HEIGHT_SM = '90%';
  static PICTURE_HEIGHT_MD = '90%';
  static PICTURE_HEIGHT_LG = '90%';
  static PICTURE_WIDTH_SM = '72%';
  static PICTURE_WIDTH_MD = '72%';
  static PICTURE_WIDTH_LG = '40%';
  static PICTURE_WIDTH_MD_ROW = '82%';
  static PICTURE_WIDTH_MD_COLUMN = '36.6%';
  static PICTURE_HEIGHT = '95%';
  static PICTURE_HALF_HEIGHT = '50%';
  /**
   * Flex basis.
   */
  static SLIDER_FLEX_BASIS = 80;
  static BAR_FLEX_BASIS = 60;
  static OPTION_FLEX_BASIS = 250;
  static IMAGE_BORDER_RADIUS = 8;
  /**
   * Edit text.
   */
  static Edit_Text = '编辑';
  /**
   * Tools name.
   */
  static toolsAndName: ToolsAndName[] = [
    {
      pic: $r("app.media.tools_2"), pic_name: "截取"
    },
    {
      pic: $r("app.media.tools_1"), pic_name: "裁剪"
    },
    {
      pic: $r("app.media.tools_3"), pic_name: "调节"
    },
    {
      pic: $r("app.media.tools_4"), pic_name: "滤镜"
    }];
  /**
   * Filter name.
   */
  static filterAndName: PicAndName[] = [
    {
      pic: $r("app.media.photo"), pic_name: "原片", color_filter: [1, 0, 0, 0, 0,
      0, 1, 0, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 1, 0]
    },
    {
      pic: $r("app.media.photo"), pic_name: "经典", color_filter: [1, 0, 0, 0, 0,
      0, 1, 0, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 1, 0]
    },
    {
      pic: $r("app.media.photo"), pic_name: "晨光", color_filter: [1, 0, 0, 0, 0,
      0, 1, 0, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 1, 0]
    },
    {
      pic: $r("app.media.photo"), pic_name: "黄昏", color_filter: [1, 0, 0, 0, 0,
      0, 1, 0, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 1, 0]
    },
    {
      pic: $r("app.media.photo"), pic_name: "光晕", color_filter: [1, 0, 0, 0, 0,
      0, 1, 0, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 1, 0]
    },
    {
      pic: $r("app.media.photo"), pic_name: "童年", color_filter: [1, 0, 0, 0, 0,
      0, 1, 0, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 1, 0]
    },
    {
      pic: $r("app.media.photo"), pic_name: "怀旧", color_filter: [1, 0, 0, 0, 0,
      0, 1, 0, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 1, 0]
    },
    {
      pic: $r("app.media.photo"), pic_name: "老照片", color_filter: [1, 1, 0, 0, 0,
      1, 1, 0, 0, 0,
      1, 0, 1, 0, 0,
      1, 0, 0, 1, 0]
    }
  ];
}