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

@Builder
export function harBuilder(value: object) {
  NavDestination() {
    Column() {
      Button($r("app.string.to_harc_pageC2"), { stateEffect: true, type: ButtonType.Capsule })
        .width('80%')
        .height(40)
        .margin(20)
        .onClick(() => {
          buildRouterModel(RouterNameConstants.ENTRY_HAP, BuilderNameConstants.HARC_C2);
        })
    }
    .width('100%')
    .height('100%')
  }
  .title('C1Page')
  .onBackPressed(() => {
    RouterModule.pop(RouterNameConstants.ENTRY_HAP);
    return true;
  })
}

const builderName = BuilderNameConstants.HARC_C1;
if (!RouterModule.getBuilder(builderName)) {
  let builder: WrappedBuilder<[object]> = wrapBuilder(harBuilder);
  RouterModule.registerBuilder(builderName, builder);
}

