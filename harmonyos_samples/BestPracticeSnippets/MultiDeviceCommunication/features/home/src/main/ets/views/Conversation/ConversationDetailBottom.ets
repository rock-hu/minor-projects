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

import { BaseConstants } from '@ohos/base/src/main/ets/constants/BaseConstants';
import { StandardIcon } from '@ohos/base/src/main/ets/views/image/StandardIcon';
import { HomeConstants } from '../../constants/HomeConstants';

@Component
export struct ConversationDetailBottom {
  @Builder CustomKeyboardBuilder(){}
  build() {
    Flex({ alignItems: ItemAlign.Center }) {
      Column() {
        StandardIcon({ icon: $r('app.media.ic_public_emoji') })
      }
      .flexBasis(HomeConstants.FLEX_BASIS_AUTO)
      .padding({
        right: $r('app.float.conversation_detail_bottom_padding'),
        left: $r('app.float.conversation_detail_bottom_padding'),
      })

      Blank()
      Column() {
        TextArea()
          .placeholderColor($r('app.color.text_input_default_color'))
          .caretColor($r('app.color.text_input_default_care_color'))
          .backgroundColor($r('app.color.text_input_default_background_color'))
          .borderRadius(HomeConstants.CONVERSATION_DETAIL_BOTTOM_TEXT_RADIUS)
          .flexGrow(BaseConstants.FLEX_GROW_ONE)
          .padding({ right: $r('app.float.conversation_detail_bottom_text_padding') })
          .backgroundColor(Color.White)
          .enableKeyboardOnFocus(false)
          .customKeyboard(this.CustomKeyboardBuilder())
      }

      Column() {
        StandardIcon({ icon: $r('app.media.ic_public_add_norm') })
      }
      .padding({
        right: $r('app.float.conversation_detail_bottom_padding'),
        left: $r('app.float.conversation_detail_bottom_padding'),
      })
    }
    .width(BaseConstants.FULL_WIDTH)
    .height($r('app.float.conversation_detail_bottom_height'))
  }
}