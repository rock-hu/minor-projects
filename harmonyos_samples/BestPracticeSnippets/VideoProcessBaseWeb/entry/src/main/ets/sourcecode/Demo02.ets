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

import { window } from '@kit.ArkUI';
import { common } from '@kit.AbilityKit';

// src/main/ets/pages/Index.ets
const uiContext: UIContext | undefined = AppStorage.get('uiContext');
let context = uiContext!.getHostContext()
// [Start component_index]
@Entry
@Component
struct Index {
  private windowClass: window.Window | null = null;
  private context: common.UIAbilityContext = context as common.UIAbilityContext;

  aboutToAppear(): void {
    window.getLastWindow(this.context).then((windowClass) => this.windowClass = windowClass);
  }

  /**
   * Changes the preferred orientation of the window.
   *
   * @param orientation - The preferred orientation to set. This should be a value from the `window.Orientation` enum.
   * @returns void - This function does not return any value.
   */
  changeOrientation(orientation: window.Orientation) {
    this.windowClass?.setPreferredOrientation(orientation);
  }

  build() {
  }
}
// [End component_index]