/*
* Copyright (C) 2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/*
* 最佳实践: 分析帧率问题
*/

// [Start child_component]
@Reusable
@Component
struct ChildComponent {
  @State item: string = '';


  aboutToReuse(params: Record<string, Object>): void {
    this.item = params.item as string;
  }


  build() {
    Stack() {
      Text(this.item)
        .fontSize(50)
        .margin({ left: 10, right: 10 })
    }
  }
}
// [End child_component]