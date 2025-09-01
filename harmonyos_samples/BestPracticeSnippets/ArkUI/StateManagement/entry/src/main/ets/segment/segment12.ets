/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

// [Start Case8]
import { ButtonComponent } from '../components/ButtonComponent';
import { ListItemComponent } from '../components/ListItemComponent';
@Entry
@Component
struct UseEmitterPublish {
  listData: string[] = ['A', 'B', 'C', 'D', 'E', 'F'];
  build() {
    Column() {
      Row() {
        Column() {
          ButtonComponent()
        }
      }
      Column() {
        Column() {
          List() {
            ForEach(this.listData, (item: string, index: number) => {
              ListItemComponent({ myItem: item, index: index })
            })
          }
          .height('100%')
          .width('100%')
          .alignListItem(ListItemAlign.Center)
        }
      }
    }
  }
}
// [End Case8]