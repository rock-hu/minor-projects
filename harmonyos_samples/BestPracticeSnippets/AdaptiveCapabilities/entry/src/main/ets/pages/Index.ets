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

import { DirectoryItem, DirectoryList } from '../common/DirectoryList'

@Entry
@Component
struct Index {
  private directory: DirectoryItem[] = [
    { title: $r('app.string.atomic_capability'), uri: 'pages/atomicLayoutCapability/AtomicLayoutCapabilityIndex', id:'AtomicLayoutCapabilityIndex' },
    { title: $r('app.string.responsive_layout'), uri: 'pages/responsiveLayout/ResponsiveLayoutIndex', id:'ResponsiveLayoutIndex' },
    { title: $r('app.string.typical_scenario'), uri: 'pages/typicalScene/TypicalSceneIndex', id:'TypicalSceneIndex' },
    { title: $r('app.string.resource_qualifier'), uri: 'pages/resourceQualifier/ResourceQualifier', id:'ResourceQualifier' },
    { title: $r('app.string.interaction_norm_capability'), uri: 'pages/interactionNormCapability/InteractionNormCapability', id:'InteractionNormCapability' }
  ]
  @State inSetValue: number = 40

  build() {
    Column() {
      DirectoryList({ title: $r('app.string.home_title'), directory: this.directory })
    }
  }
}