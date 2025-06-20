/*
* Copyright (C) 2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import { BuilderNameConstants, buildRouterModel, RouterModule, RouterNameConstants, } from '@ohos/routermodule';

// [Start function_har_builder_a1]
@Builder
export function harBuilder(value: object) {
  NavDestination() {
    Column() {
      // [StartExclude function_har_builder_a1]
      Text(`传入的参数：${JSON.stringify(value)}`)
        .margin(20)
      Button($r("app.string.to_index"), { stateEffect: true, type: ButtonType.Capsule })
        .width('80%')
        .height(40)
        .margin(20)
        .onClick(() => {
          RouterModule.clear(RouterNameConstants.ENTRY_HAP);
        })
      Button($r("app.string.to_hara_pageA2"), { stateEffect: true, type: ButtonType.Capsule })
        .width('80%')
        .height(40)
        .margin(20)
        .onClick(() => {
          buildRouterModel(RouterNameConstants.ENTRY_HAP, BuilderNameConstants.HARA_A2);
        })
      // [EndExclude function_har_builder_a1]
      Button($r("app.string.to_harb_pageB1"), { stateEffect: true, type: ButtonType.Capsule })
        .width('80%')
        .height(40)
        .margin(20)
        .onClick(() => {
          buildRouterModel(RouterNameConstants.ENTRY_HAP, BuilderNameConstants.HARB_B1);
        })
    }
    .width('100%')
    .height('100%')
  }
  .title('A1Page')
  .onBackPressed(() => {
    RouterModule.pop(RouterNameConstants.ENTRY_HAP);
    return true;
  })
}
// [End function_har_builder_a1]

const builderName = BuilderNameConstants.HARA_A1;
if (!RouterModule.getBuilder(builderName)) {
  const builder: WrappedBuilder<[object]> = wrapBuilder(harBuilder);
  RouterModule.registerBuilder(builderName, builder);
}


