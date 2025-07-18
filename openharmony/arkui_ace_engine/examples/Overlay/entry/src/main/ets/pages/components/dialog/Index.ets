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

import { NavList, NavListItem } from '../../utils/navigationList'

@Builder
export function DialogIndexBuilder(name: string, param: Object) {
  DialogIndex()
}

@Entry
@Component
export struct DialogIndex {
  pathStack: NavPathStack = new NavPathStack()
  paths: NavListItem[] = [
    { name: 'AlertDialog', path: 'AlertDialog' },
    { name: 'ActionSheet', path: 'ActionSheet' },
    { name: 'CustomDialog', path: 'CustomDialog' },
    { name: 'PromptAction', path: 'PromptAction' },
    { name: 'DialogController', path: 'DialogController' },
    { name: 'DialogLevelOrder', path: 'DialogLevelOrder' },
    { name: 'DialogFocusable', path: 'DialogFocusable' },
    { name: 'EmbeddedDialog', path: 'EmbeddedDialogPageOne' },
    { name: 'AlertDialogAvoid', path: 'AlertDialogAvoid' },
    { name: 'CustomDialogAvoid', path: 'CustomDialogAvoid' },
    { name: 'Dialog组合场景1', path: 'DialogLevel0' }
  ]

  build() {
    NavDestination() {
      Scroll() {
        NavList({
          pages: this.paths,
          onPathChange: (item: NavListItem) => {
            this.pathStack.pushPath( { name: item.path } )
          }
        })
      }
    }
    .title('Dialog')
    .onBackPressed(() => {
      this.pathStack.pop()
      return true
    })
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}