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

import { DirectoryItem, DirectoryList } from '../../common/DirectoryList'

@Entry
@Component
struct TypicalSceneIndex {
  private typicalScene: Array<DirectoryItem> = [
    { title: $r('app.string.diversion_layout'), uri: 'pages/typicalScene/diversion/DiversionSample', id:'DiversionSample' },
    { title: $r('app.string.repeat_layout'), uri: 'pages/typicalScene/repeat/RepeatSample', id:'RepeatSample' },
    { title: $r('app.string.indentation_layout'), uri: 'pages/typicalScene/indentation/IndentationSample', id:'IndentationSample' },
    { title: $r('app.string.header'), uri: 'pages/typicalScene/header/HeaderSample', id:'HeaderSample' },
    { title: $r('app.string.banner'), uri: 'pages/typicalScene/banner/BannerSample', id:'BannerSample' },
    { title: $r('app.string.operationEntries'), uri: 'pages/typicalScene/operationEntries/OperationEntriesSample', id:'OperationEntriesSample' },
    { title: $r('app.string.tabs'), uri: 'pages/typicalScene/tabs/TabsSample', id:'TabsSample' },
    { title: $r('app.string.bigImage'), uri: 'pages/typicalScene/bigImage/BigImageSample', id:'BigImageSample' },
    { title: $r('app.string.multiLaneList'), uri: 'pages/typicalScene/multiLaneList/MultiLaneListSample', id:'MultiLaneListSample' },
    { title: $r('app.string.grid'), uri: 'pages/typicalScene/responsiveGrid/ResponsiveGridSample', id:'ResponsiveGridSample' },
    { title: $r('app.string.aside'), uri: 'pages/typicalScene/sidebar/SidebarSample', id:'SidebarSample' },
    { title: $r('app.string.multiScene'), uri: 'pages/typicalScene/multiScene/pages/MultiScene', id:'multiScene' }
  ]

  build() {
    Column() {
      DirectoryList({ title: $r('app.string.typical_scenario'), directory: this.typicalScene })
    }
  }
}