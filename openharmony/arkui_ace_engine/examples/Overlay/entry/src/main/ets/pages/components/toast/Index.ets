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
export function ToastIndexBuilder(name: string, param: Object) {
  ToastIndex()
}

@Entry
@Component
export struct ToastIndex {
  pathStack: NavPathStack = new NavPathStack()
  paths: NavListItem[] = [
    { name: 'Toast', path: 'Toast' },
    { name: 'showToastTest', path: 'showToastTest' },
    { name: 'toastPositionTest', path: 'toastPositionTest' },
  ]

  build() {
    NavDestination() {
      NavList({
        pages: this.paths,
        onPathChange: (item: NavListItem) => {
          this.pathStack.pushPath( { name: item.path } )
        }
      })
    }
    .title('Toast')
    .onBackPressed(() => {
      this.pathStack.pop()
      return true
    })
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}