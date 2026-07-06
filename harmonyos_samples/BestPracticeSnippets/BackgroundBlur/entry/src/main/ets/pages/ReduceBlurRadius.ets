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

import { window } from "@kit.ArkUI";

@Component
export struct ReduceBlurRadius {
  @Consume('navPathStack') navPathStack: NavPathStack;
  @State text: string = 'Hello World';
  @State dataSource: ImageDataSource = new ImageDataSource(dataList());

  build() {
    NavDestination() {
      List({ space: 10, initialIndex: 0 }) {
        LazyForEach(this.dataSource, () => {
          ListItem() {
            TextItem()
          }
        })
      }
      .width('100%')
      .height('100%')
      .divider({
        strokeWidth: 2,
        color: 0xFFFFFF,
        startMargin: 20,
        endMargin: 20
      })
    }
    .hideTitleBar(true)
  }
}

@Component
struct TextItem {
  array: string[] = [];

  aboutToAppear(): void {
    for (let index = 0; index < 20; index++) {
      this.array.push('A' + index.toString());
    }
  }

  build() {
    Row() {
      ForEach(this.array, () => {
        BlurItemList()
      })
    }
  }
}

@Component
export struct BlurItemList {
  @State bottomSafeHeight: number = 0; // Bottom navigation bar height.

  aboutToAppear(): void {
    window.getLastWindow(this.getUIContext().getHostContext()!, (err, windowBar) => {
      if (err.code) {
        return;
      }
      // get the height of the bottom navigation bar.
      this.bottomSafeHeight = this.getUIContext()
        .px2vp(windowBar.getWindowAvoidArea(window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR).bottomRect.height);
      windowBar.setWindowLayoutFullScreen(true);
    })
  }

  build() {
    Column() {
      // [Start ReduceBlurRadius]
      Text('Backdrop Blur')
        .padding(5)
        .width('90%')
        .height('50%')
        .fontSize(2)
        .fontColor(Color.White)
        .textAlign(TextAlign.Center)
        .backdropBlur(0.2) // The blur radius is reduced to 0.2,the specific value depends on actual requirements.
        .backgroundImage($r('app.media.test')) // Here 'app.media.test' is just an example, developers should replace it with their own.
        .backgroundImageSize({ width: '90%' })
        .backgroundImagePosition(Alignment.Center)
      // [Start ReduceBlurRadius]
    }
    .padding({ bottom: this.bottomSafeHeight + 16 })
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
  }
}

function dataList() {
  let element: string[] = [];
  for (let i = 0; i < 100000; i++) {
    element.push(i.toString());
  }
  return element;
}

class ImageDataSource implements IDataSource {
  private listeners: DataChangeListener[] = [];
  private originDataArray: string[] = [];

  constructor(dataSource: string[]) {
    this.originDataArray = dataSource;
  }

  totalCount(): number {
    return this.originDataArray.length;
  }

  getData(index: number): string {
    return this.originDataArray[index];
  }

  registerDataChangeListener(listener: DataChangeListener): void {
    if (this.listeners.indexOf(listener) < 0) {
      console.info('add listener');
      this.listeners.push(listener);
    }
  }

  unregisterDataChangeListener(listener: DataChangeListener): void {
    const pos = this.listeners.indexOf(listener);
    if (pos >= 0) {
      console.info('remove ');
    }
  }
}