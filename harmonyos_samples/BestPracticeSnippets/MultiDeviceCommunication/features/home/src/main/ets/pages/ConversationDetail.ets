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

import { Adaptive, BaseConstants, BreakpointConstants } from '@ohos/base';
import { ConversationDetailBottom } from '../views/Conversation/ConversationDetailBottom';
import ConversationDetailItem from '../views/Conversation/ConversationDetailItem';
import { ConversationDetailTopSearch } from '../views/Conversation/ConversationDetailTopSearch';
import { HomeConstants } from '../constants/HomeConstants';
import { deviceInfo } from '@kit.BasicServicesKit';
// [Start conversation_detail]
@Component
export struct ConversationDetail {
  @StorageProp('currentBreakpoint') currentBreakpoint: string = BreakpointConstants.BREAKPOINT_SM;
  @Prop currentConversationUserName: string;
  @Prop currentFeatureIndex: number;
  @Consume('pageInfo') pageInfo: NavPathStack;

  build() {
    NavDestination() {
      Flex({ direction: FlexDirection.Column }) {
        ConversationDetailTopSearch({ currentConversationUserName: $currentConversationUserName, })
          .height(Adaptive.ContactItemHeight(this.currentBreakpoint))
        if (this.currentConversationUserName.length === 3) {
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: true,
            content: "Fen-fen invited you for milk tea",
            isAppletMsg: true,
            currentFeatureIndex: $currentFeatureIndex
          })
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: true,
            content: "Please drink milk tea, quickly order it!! Speed, speed, all that's missing is you",
            currentFeatureIndex: $currentFeatureIndex
          })
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: false,
            content: "Ha-ha, what's the happy news today",
            currentFeatureIndex: $currentFeatureIndex
          })
        } else {
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: true,
            content: "Marketing Department 2023 new product promotion plan",
            isDocumentMsg: HomeConstants.IS_DOCUMENT_CONVERSATION,
            currentFeatureIndex: $currentFeatureIndex
          })
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: true,
            content: "This is the material for tomorrow's meeting. Please send it to Jack for his confirmation",
            currentFeatureIndex: $currentFeatureIndex
          })
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: false,
            content: "Ok, copy that",
            currentFeatureIndex: $currentFeatureIndex
          })
        }
        Blank()
        ConversationDetailBottom()
      }
      .height('100%')
      .width('100%')
      .backgroundColor('#F6F6F6')
      .padding({
        bottom: deviceInfo.deviceType !== BaseConstants.DEVICE_2IN1 ? '28vp' : 0
      })
    }
    .hideTitleBar(true)
  }
}
// [End conversation_detail]