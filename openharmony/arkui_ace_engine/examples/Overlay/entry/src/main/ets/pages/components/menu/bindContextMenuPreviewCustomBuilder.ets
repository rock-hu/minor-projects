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

@Builder
export function BindContextMenuPreviewCustomBuilder(name: string, param: Object) {
  BindContextMenuPreviewCustomBuilderExample()
}

@Entry
@Component
struct BindContextMenuPreviewCustomBuilderExample {
  private iconStr: ResourceStr = $r('app.media.app_icon')
  pathStack: NavPathStack = new NavPathStack()

  @Builder
  MyMenu() {
    Menu() {
      MenuItem({ startIcon: this.iconStr, content: '菜单选项' })
      MenuItem({ startIcon: this.iconStr, content: '菜单选项' })
      MenuItem({ startIcon: this.iconStr, content: '菜单选项' })
    }
  }

  @Builder
  MyPreview() {
    Column() {
      Image($r('app.media.app_icon'))
        .width(200)
        .height(200)
    }
  }

  build() {
    NavDestination() {
      Column({ space: 50 }) {
        Column() {
          Column() {
            Text('preview-builder')
              .width(200)
              .height(100)
              .textAlign(TextAlign.Center)
              .margin(100)
              .fontSize(30)
              .bindContextMenu(this.MyMenu, ResponseType.LongPress,
                {
                  preview: this.MyPreview
                })
          }
        }.width('100%')
      }
    }
    .title('BindContextMenuPreviewCustomBuilder')
    .onBackPressed(() => {
      this.pathStack.pop()
      return true
    })
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}