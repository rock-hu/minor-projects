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

import { Constants } from '../constant/Constant';
import { LaneData } from '../model/RouteDataModel';
import { BreakpointType } from '../utils/BreakpointUtil';
import { RoadView } from '../view/RoadView';

let storage: LocalStorage = LocalStorage.getShared();

@Entry(storage)
@Component
struct Index {
  // [Start layoutBasedOnVerticalBreakpoint]
  @StorageLink('verticalBreakpoint') verticalBreakpoint: string = Constants.BREAK_POINT_SM;
  @LocalStorageProp('laneData') roadInfo: LaneData | undefined = undefined;

  build() {
    Stack({ alignContent: Alignment.Top }) {
      RoadView({ laneNum: this.roadInfo?.laneNum })

      Image($rawfile('traffic_light.svg'))
        .objectFit(ImageFit.Contain)
        .width(80)
        .position({
          // Layout based on vertical breakpoint.
          right: new BreakpointType(25, 25, 75).getValue(this.verticalBreakpoint),
          top: 25
        })
    }
    .width('100%')
    .height('100%')
  }
  // [End layoutBasedOnVerticalBreakpoint]
}