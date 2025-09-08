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

// [Start ImproveReuseHitRate]
import { MyDataSource } from '../common/CommonConstants';

@Observed
class Class1 {
  public typeValue: string = '';
  public id: string = '';
}

@Entry
@Component
struct ImproveReuseHitRate {
  private data: MyDataSource = new MyDataSource();

  aboutToAppear(): void {
    // Init Data.
  }

  build() {
    List({ space: 3 }) {
      LazyForEach(this.data, (item: Class1) => {
        ReusableComponent({ value1: item }).reuseId(item.typeValue)
      }, (item: Class1) => item.id)
    }.cachedCount(5)
  }
}

@Reusable
@Component
struct ReusableComponent {
  @ObjectLink value1: Class1;

  aboutToAppear(): void {
    // Do some init.
  }

  build() {
    ListItem() {
      ContentBuilder({ value1: this.value1 })
    }
  }
}

interface ContentParamType {
  value1: Class1
}

@Builder
function ContentBuilder($$: ContentParamType) {
  if ($$.value1.typeValue === 'A') {
    // BuildTypeA
  } else if ($$.value1.typeValue === 'C') {
    // BuildTypeC
  } else if ($$.value1.typeValue === 'D') {
    // BuildTypeD
  } else if ($$.value1.typeValue === 'E') {
    // BuildTypeE
  } else if ($$.value1.typeValue === 'F') {
    // BuildTypeF
  } else if ($$.value1.typeValue === 'G') {
    // BuildTypeG
  } else if ($$.value1.typeValue === 'H') {
    // BuildTypeH
  } else if ($$.value1.typeValue === 'I') {
    // BuildTypeI
  } else if ($$.value1.typeValue === 'J') {
    // BuildTypeJ
  } else if ($$.value1.typeValue === 'K') {
    // BuildTypeK
  }
}

@Builder
function TypeA($$: ContentParamType) {

  Column() {
    Title()
      .visibility(Visibility.Visible) // Control the display or concealment of components.
    Row() {
      Column() {
        Image($r('app.media.startIcon'))
        Text()
        Text()
        Text()
      }
    }
  }
}

@Builder
function TypeC($$: ContentParamType) {
  Column() {
    Title()
    Grid() {
      // ...
    }
  }
}


@Component
struct Title {
  build() {
    Column() {
      Text('title')
    }
  }
}

// [End ImproveReuseHitRate]


