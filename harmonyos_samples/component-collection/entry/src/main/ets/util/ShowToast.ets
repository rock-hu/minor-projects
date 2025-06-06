/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { promptAction } from '@kit.ArkUI';

const uiContext: UIContext | undefined = AppStorage.get('uiContext');

class ShowToast {
  private longToastTime: number = 3000;
  private shortToastTime: number = 1000;

  showToast(message: ResourceStr, duration: number) {
    uiContext?.getPromptAction().showToast({ message: message, duration: duration });
  }

  shortToast(message: ResourceStr) {
    this.showToast(message, this.shortToastTime);
  }

  longToast(message: ResourceStr) {
    this.showToast(message, this.longToastTime);
  }
}

export default new ShowToast();