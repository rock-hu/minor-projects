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

import { hilog } from '@kit.PerformanceAnalysisKit';
import MediaItem, { ItemType } from '../model/MediaItem';
import { StickyWaterFlowDataSource } from '../model/StickyWaterFlowDataSource';

const TAG = 'StickyPage';


@Component
struct FlowVideoItem {
  @Prop item: MediaItem;
  controller: VideoController = new VideoController();

  aboutToReuse(params: Record<string, MediaItem>) {
    this.item = params.item as MediaItem;
  }

  build() {
    if (this.item.type === ItemType.VIDEO) {
      Stack({ alignContent: Alignment.BottomStart }) {
        Video({ src: this.item.videoUri, previewUri: this.item.videoCover, controller: this.controller })
          .controls(false)
          .muted(true)
          .loop(true)
          .borderRadius(8)
          .onVisibleAreaChange([0.0, 1.0], (isVisible: boolean, currentRatio: number) => {
            if (isVisible && currentRatio >= 1.0) {
              this.controller.start();
            }
            if (!isVisible || currentRatio < 1.0) {
              this.controller.pause();
            }
          })
        Text('NO. ' + (this.item.id + 1))
          .fontSize(12)
          .fontColor(Color.White)
          .margin({
            left: 8,
            bottom: 4
          })
      }
    } else {
      RelativeContainer() {
        Image(this.item.imageUri)
          .objectFit(ImageFit.Cover)
          .width('100%')
          .layoutWeight(1)
          .borderRadius(8)
          .alignRules({
            top: { anchor: '__container__', align: VerticalAlign.Top },
            bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
            left: { anchor: '__container__', align: HorizontalAlign.Start },
            right: { anchor: '__container__', align: HorizontalAlign.End }
          })
          .id('image')

        Stack() {}
        .linearGradient({
          angle: 0,
          colors: [['#80000000', 0.0],
            ['#03000000', 1.0]]
        })
        .width('100%')
        .height(32)
        .borderRadius(8)
        .hitTestBehavior(HitTestMode.None)
        .alignRules({
          bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
          left: { anchor: '__container__', align: HorizontalAlign.Start },
          right: { anchor: '__container__', align: HorizontalAlign.End }
        })
        .id('mask')

        Text('NO. ' + (this.item.id + 1))
          .fontSize(12)
          .fontColor(Color.White)
          .alignRules({
            bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
            left: { anchor: '__container__', align: HorizontalAlign.Start }
          })
          .margin({
            left: 8,
            bottom: 4
          })
          .id('text')
      }
      .width('100%')
      .borderRadius(8)
      .backgroundColor(Color.Gray)
    }
  }
}

@Entry
@Component
export struct StickyPage {
  @State arr: number[] = [1, 2, 3, 4, 5, 6];
  @State colors: number[] = [0xFFC0CB, 0xDA70D6, 0x6B8E23, 0x6A5ACD, 0x00FFFF, 0x00FF7F];
  @State scrollOffset: number = 0;
  @State minSize: number = 80;
  @State maxSize: number = 210;
  @State sections: WaterFlowSections = new WaterFlowSections();
  @State isFoldStatus: boolean = true;

  scroller: Scroller = new Scroller();
  dataSource: StickyWaterFlowDataSource = new StickyWaterFlowDataSource();
  private itemWidthArray: number[] = [];
  private itemHeightArray: number[] = [];
  oneColumnSection: SectionOptions = {
    itemsCount: 3,
    crossCount: 1,
    columnsGap: 5,
    rowsGap: 10,
    margin: {
      top: 8,
      left: 0,
      bottom: 8,
      right: 0
    },
    onGetItemMainSizeByIndex: (index: number) => {
      if (index === 1) {
        return 150;
      } else {
        return 220;
      }
    }
  };
  twoColumnSection: SectionOptions = {
    itemsCount: 2,
    crossCount: 2,
    onGetItemMainSizeByIndex: (index: number) => {
      return 270;
    }
  };

  getResourceStringArray(resource: Resource): Array<string> {
    let result: Array<string> = new Array<string>();
    try {
      result = this.getUIContext().getHostContext()?.resourceManager.getStringArrayValueSync(resource.id) as string[];
    } catch (e) {
      hilog.error(0x0000, TAG, `[getResourceString]getStringSync failed, error:${JSON.stringify(e)}.`);
    }
    return result;
  }

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

  aboutToAppear() {
    this.setItemSizeArray();
    let sectionOptions: SectionOptions[] = [];
    let count = 0;
    let oneOrTwo = 0;
    while (count < 100) {
      if (oneOrTwo++ % 2 === 0) {
        sectionOptions.push(this.oneColumnSection);
        count += this.oneColumnSection.itemsCount;
      } else {
        sectionOptions.push(this.twoColumnSection);
        count += this.twoColumnSection.itemsCount;
      }
    }
    this.sections.splice(-1, 0, sectionOptions);
  }

  build() {
    Stack({ alignContent: Alignment.TopStart }) {
      WaterFlow({ scroller: this.scroller, sections: this.sections }) {
        LazyForEach(this.dataSource, (item: MediaItem) => {
          FlowItem() {
            FlowVideoItem({ item: item })
          }
          .width('100%')
          .height(this.itemHeightArray[item.id % 100])
          .backgroundColor(Color.White)
        }, (item: MediaItem) => item.id.toString())
      }
      .cachedCount(12)
      .columnsTemplate('1fr 1fr')
      .columnsGap(8)
      .rowsGap(8)
      .padding({ bottom: '48vp' })
      .width('100%')
      .height('100%')
      .layoutWeight(1)
    }
  }
}