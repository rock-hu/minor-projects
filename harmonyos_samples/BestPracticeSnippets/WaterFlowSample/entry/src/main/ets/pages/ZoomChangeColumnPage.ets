/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import { MyDataSource } from '../model/MyDataSource'
import { image } from '@kit.ImageKit';

@Reusable
@Component
struct ReusableItem {
  @State item: number = 0;

  aboutToReuse(params: Record<string, number>): void {
    this.item = params.item;
    console.log('Reuse item: ' + this.item);
  }

  build() {
    Column() {
      Text('N ' + this.item)
        .fontSize(12)
        .height('16vp')
      Image($rawfile(`sections/${this.item % 4}.jpg`))
        .objectFit(ImageFit.Fill)
        .width('100%')
        .layoutWeight(1)
    }
  }
}

@Entry
@Component
struct ZoomChangeColumnPage {
  @State minSize: number = 80
  @State maxSize: number = 210;
  @State fontSize: number = 24;
  @State colors: number[] = [0xFFC0CB, 0xDA70D6, 0x6B8E23, 0x6A5ACD, 0x00FFFF, 0x00FF7E];
  @State columns: number = 4;
  @State itemScale: number = 1;
  @State imageScale: number = 1;
  @State itemOpacity: number = 1;
  @State gestureEnd: boolean = false;
  private pinchTime: number = 0;
  private data: MyDataSource = new MyDataSource();
  private itemWidthArray: number[] = [];
  private itemHeightArray: number[] = [];
  @State pixelMap: image.PixelMap | undefined = undefined;
  private columnChanged: boolean = false;
  private oldColumn: number = this.columns;
  @State sections: WaterFlowSections = new WaterFlowSections();

  getSize() {
    let ret = Math.floor(Math.random() * this.maxSize);
    return (ret > this.minSize ? ret : this.minSize);
  }

  setItemSizeArray() {
    for (let i = 0; i < 100; i++) {
      this.itemWidthArray.push(this.getSize());
      this.itemHeightArray.push(this.getSize());
    }
  }

  aboutToAppear(): void {
    for(let i = 0; i < 100; i++) {
      this.data.pushData(i);
    }
    let lastCount = AppStorage.get<number>('columnsCount');
    if (typeof lastCount != 'undefined') {
      this.columns = lastCount;
    }
    this.setItemSizeArray();
  }

  changeColumns(scale: number) {
    this.oldColumn = this.columns;
    if (scale > (this.columns / (this.columns - 0.5))) {
      this.columns--;
      this.columnChanged = true;
    } else if (scale < 1 && this.columns < 4) {
      this.columns++;
      this.columnChanged = true;
    }
    this.columns = Math.min(4, Math.max(1, this.columns));
  }

  build() {
    Column({ space: 2 }) {
      Row() {
        Text('双指缩放改变列数')
          .height('5%')
          .margin({ top: 10, left: 20 })
      }
      Stack() {
        Image(this.pixelMap)
          .width('100%')
          .height('100%')
          .scale({
            x: this.imageScale,
            y: this.imageScale,
            centerX: 0,
            centerY: 0
          })
        WaterFlow() {
          LazyForEach(this.data, (item: number) => {
            FlowItem() {
              ReusableItem({ item: item })
            }
            .width('100%')
            .aspectRatio(this.itemHeightArray[item % 100] / this.itemWidthArray[item % 100])
            .backgroundColor(this.colors[item % 5])
          }, (item: number) => item.toString())
        }
        .id('waterflow')
        .columnsTemplate('1fr '.repeat(this.columns))
        .backgroundColor(0xFAEEE0)
        .width('100%')
        .height('100%')
        .layoutWeight(1)
        .opacity(this.itemOpacity)
        .scale({
          x: this.itemScale,
          y: this.itemScale,
          centerX: 0,
          centerY: 0
        })
        .priorityGesture(
          PinchGesture()
            .onActionStart((event: GestureEvent) => {
              this.gestureEnd = false;
              this.pinchTime = event.timestamp;
              this.columnChanged = false;
              this.getUIContext().getComponentSnapshot().get('waterflow', (error: Error, pixelMap: image.PixelMap) => {
                if (error) {
                  console.info('error: ' + error.message);
                  return;
                }
                this.pixelMap = pixelMap;
              })
            })
            .onActionUpdate((event: GestureEvent) => {
              if (event.timestamp - this.pinchTime < 10000000) {
                return;
              }
              this.pinchTime = event.timestamp;
              let maxScale = this.oldColumn / (this.oldColumn - 1);
              this.itemScale = event.scale > maxScale ? maxScale : event.scale;
              this.imageScale = event.scale > maxScale ? maxScale : event.scale;
              this.itemOpacity = (this.itemScale > 1) ? (this.itemScale - 1) : (1 - this.itemScale);
              this.itemOpacity *= 3;
              if (!this.columnChanged) {
                this.changeColumns(event.scale);
              }
              if (this.columnChanged) {
                this.itemScale = this.imageScale * this.columns / this.oldColumn;
                if (event.scale < 1) {
                  this.itemScale = this.itemScale > 1 ? this.itemScale : 1;
                } else {
                  this.itemScale = this.itemScale < 1 ? this.itemScale : 1;
                }
              }
            })
            .onActionEnd((event: GestureEvent) => {
              this.gestureEnd = true;
              this.getUIContext().animateTo({ duration: 300 }, () => {
                this.itemScale = 1;
                this.itemOpacity = 1;
              });
              AppStorage.setOrCreate<number>('columnsCount', this.columns);
            })
        )
      }
      .width('100%')
      .height('100%')
    }
  }
}

