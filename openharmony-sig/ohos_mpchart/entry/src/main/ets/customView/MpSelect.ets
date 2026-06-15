/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

@ComponentV2
export default struct MpSelect {
  @Require @Param name: string | Resource
  @Require @Param selectItem: Object
  @Event $selectItem: (selectItem: Object) => void = (selectItem: Object) => {};
  @Require @Param itemsList: Array<Object>
  @Require @Param valuesList: Array<ValueObject>

  build() {
    Row() {
      Text(this.name)
      Blank()
      Select(this.valuesList)
        .value(this.valuesList[0].value)
        .onSelect((index: number) => {
          this.$selectItem(this.itemsList[index]);
        })
    }
    .width('100%')
  }
}

export class ValueObject {
  value: string | Resource = ''
}

