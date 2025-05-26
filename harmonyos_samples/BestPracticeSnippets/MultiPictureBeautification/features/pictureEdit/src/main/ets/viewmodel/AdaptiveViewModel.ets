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

import { BreakpointType } from '@ohos/commons';
import PictureEditConstants from '../constants/PictureEditConstants';

export class Adaptive {
  static PICTURE_HEIGHT = (currentBreakpoint: string): string => {
    return new BreakpointType(
      PictureEditConstants.PICTURE_HEIGHT_SM,
      PictureEditConstants.PICTURE_HEIGHT_MD,
      PictureEditConstants.PICTURE_HEIGHT_LG,
    ).GetValue(currentBreakpoint);
  }
  static PICTURE_WIDTH = (currentBreakpoint: string): string => {
    return new BreakpointType(
      PictureEditConstants.PICTURE_WIDTH_SM,
      PictureEditConstants.PICTURE_WIDTH_MD,
      PictureEditConstants.PICTURE_WIDTH_LG,
    ).GetValue(currentBreakpoint);
  }
}
export interface ToolsAndName {
  pic: Resource
  pic_name: string
}

export interface PicAndName {
  pic: Resource
  pic_name: string
  color_filter: number[]
}