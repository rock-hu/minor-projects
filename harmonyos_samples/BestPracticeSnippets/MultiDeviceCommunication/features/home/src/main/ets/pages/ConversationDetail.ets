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
        if (this.currentConversationUserName.length === HomeConstants.CONVERSATION_DEFAULT_SIZE) {
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: HomeConstants.IS_RECEIVED,
            content: HomeConstants.CONVERSATION_LIST_APPLET[0],
            isAppletMsg: HomeConstants.IS_APPLET_CONVERSATION,
            currentFeatureIndex: $currentFeatureIndex
          })
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: HomeConstants.IS_RECEIVED,
            content: HomeConstants.CONVERSATION_LIST_APPLET[1],
            currentFeatureIndex: $currentFeatureIndex
          })
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: !HomeConstants.IS_RECEIVED,
            content: HomeConstants.CONVERSATION_LIST_APPLET[2],
            currentFeatureIndex: $currentFeatureIndex
          })
        } else {
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: HomeConstants.IS_RECEIVED,
            content: HomeConstants.CONVERSATION_LIST_DOCUMENT[0],
            isDocumentMsg: HomeConstants.IS_DOCUMENT_CONVERSATION,
            currentFeatureIndex: $currentFeatureIndex
          })
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: HomeConstants.IS_RECEIVED,
            content: HomeConstants.CONVERSATION_LIST_DOCUMENT[1],
            currentFeatureIndex: $currentFeatureIndex
          })
          ConversationDetailItem({
            receivedName: $currentConversationUserName,
            isReceived: !HomeConstants.IS_RECEIVED,
            content: HomeConstants.CONVERSATION_LIST_DOCUMENT[2],
            currentFeatureIndex: $currentFeatureIndex
          })
        }
        Blank()
        ConversationDetailBottom()
      }
      .height(BaseConstants.FULL_HEIGHT)
      .width(BaseConstants.FULL_WIDTH)
      .backgroundColor($r('app.color.background_color_grey'))
      .padding({
        bottom: deviceInfo.deviceType !== BaseConstants.DEVICE_2IN1 ?
        $r('app.float.tab_content_pb') : $r('app.float.zero')
      })
    }
    .hideTitleBar(true)
  }
}
// [End conversation_detail]