/*
 *   Copyright (c) 2024 Huawei Device Co., Ltd.
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

/**
 * 最佳实践：基于HMRouter路由框架的页面跳转开发实践
 */

import { HMRouter, HMRouterMgr } from '@hadss/hmrouter';
import { PageConstant } from '../../constant/PageConstant';

/**
 * privacy background
 */

// [Start HMRouter_dialog]
@HMRouter({ pageUrl: 'privacyDialog', dialog: true })
@Component
export struct PrivacyDialogContent {
  // [StartExclude HMRouter_dialog]
  scroller: Scroller = new Scroller();

  aboutToAppear(): void {
    HMRouterMgr.push({
      pageUrl: 'PrivacyDialogDetail'
    });
  }

  build() {
    Stack({ alignContent: Alignment.Center }) {
      Column()
        .width('100%')
        .height('100%')
        .backgroundColor('rgba(0,0,0,0.5)')
        .expandSafeArea([SafeAreaType.SYSTEM], [SafeAreaEdge.TOP, SafeAreaEdge.BOTTOM]);
    };
  }

  // [EndExclude HMRouter_dialog]
}

// [End HMRouter_dialog]