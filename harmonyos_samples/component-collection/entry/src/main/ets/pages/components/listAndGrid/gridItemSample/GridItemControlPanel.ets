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

import { CommonItemSelect } from '../../../../common/AttributeModificationTool';

@Component
export struct PropertiesControlPanel {
  @Link rowStart: number;
  @Link rowEnd: number;
  @Link columnStart: number;
  @Link columnEnd: number;

  build() {
    Column() {
      CommonItemSelect({
        name: $r('app.string.grid_item_row_start'),
        selects: ['0', '1', '2', '3', '4'],
        callback: (index, value) => {
          this.rowStart = Number.parseInt(value as string);
        },
        testId: `griditem_001`
      })

      CommonItemSelect({
        name: $r('app.string.grid_item_row_end'),
        selects: ['0', '1', '2', '3', '4'],
        callback: (index, value) => {
          this.rowEnd = Number.parseInt(value as string);
        },
        testId: `griditem_002`
      })

      CommonItemSelect({
        name: $r('app.string.grid_item_column_start'),
        selects: ['0', '1', '2', '3', '4'],
        callback: (index, value) => {
          this.columnStart = Number.parseInt(value as string);
        },
        testId: `griditem_003`
      })

      CommonItemSelect({
        name: $r('app.string.grid_item_column_end'),
        selects: ['0', '1', '2', '3', '4'],
        callback: (index, value) => {
          this.columnEnd = Number.parseInt(value as string);
        },
        testId: `griditem_004`
      })
    }
  }
}