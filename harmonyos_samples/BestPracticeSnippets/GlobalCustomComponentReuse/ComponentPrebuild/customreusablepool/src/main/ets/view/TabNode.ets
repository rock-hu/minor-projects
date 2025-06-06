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
import { SecureRandom } from '../view/SecureRandom'
import { flowItemBuilder, FlowItemNode } from './FlowItemNode';
import { ViewItem } from '../data/ViewItem';
import { WaterFlowDataSource } from '../data/WaterFlowDataSource';
import { swiperBuilder, SwiperNode } from './SwiperView';
import { banners1, banners2 } from '../data/MockData';
import { avatarData, furnitureData, goodsData, natureData, travelData } from '../data/MockData';

// 瀑布流子组件WrappedBuilder对象
export const flowItemWrapper: WrappedBuilder<ESObject> = wrapBuilder<ESObject>(flowItemBuilder);

// 瀑布流轮播图WrappedBuilder对象
const swiperWrapper: WrappedBuilder<ESObject> = wrapBuilder<ESObject>(swiperBuilder);

// 自定义组件复用池Swiper页面
@Component
export struct TabNode {
  minSize: number = 80;
  maxSize: number = 180;
  fontSize: number = 24;
  @State index: number = 0;
  scroller: Scroller = new Scroller();
  dataSource: WaterFlowDataSource = new WaterFlowDataSource();
  // FlowItem组件高度列表
  private itemHeightArray: number[] = [];
  // FlowItem组件宽度列表
  private itemWidthArray: number[] = [];

  // 计算FlowItem宽/高
  getSize() {
    let ret = Math.floor(new SecureRandom().nextInt(this.maxSize));
    return (ret > this.minSize ? ret : this.minSize);
  }

  // 设置FlowItem的宽/高数组
  setItemSizeArray() {
    for (let i = 0; i < 100; i++) {
      this.itemWidthArray.push(this.getSize());
      this.itemHeightArray.push(this.getSize());
    }
  }

  aboutToAppear(): void {
    this.setItemSizeArray();
    // 添加模拟数据
    switch (this.index) {
      case 0:
        this.dataSource.addItems(furnitureData());
        break;
      case 1:
        this.dataSource.addItems(natureData());
        break;
      case 2:
        this.dataSource.addItems(goodsData());
        break;
      case 3:
        this.dataSource.addItems(travelData());
        break;
      case 4:
        this.dataSource.addItems(avatarData());
        break;
    }
  }

  // 更新数据方法
  fillNewData(item: ViewItem) {
    this.dataSource.addLastItem(item);
  }

  build() {
    Scroll(this.scroller) {
      Column({ space: 2 }) {
        SwiperNode({
          type: 'reuse_type_swiper_',
          images: this.index % 2 === 0 ? banners1 : banners2,
          builder: swiperWrapper
        })
        WaterFlow() {
          LazyForEach(this.dataSource, (item: ViewItem, index: number) => {
            FlowItem() {
              FlowItemNode({
                type: 'reuse_type_',
                data: item,
                itemColor: Color.White,
                builder: flowItemWrapper
              })
            }
            .width('100%')
          }, (item: string) => item)
        }
        .columnsTemplate("1fr 1fr")
        .columnsGap(10)
        .rowsGap(5)
        .backgroundColor('#FAEEE0')
        .width('100%')
        .height('100%')
        .nestedScroll({
          scrollForward: NestedScrollMode.PARENT_FIRST,
          scrollBackward: NestedScrollMode.SELF_FIRST
        })
      }
    }
    .width('100%')
    .height('100%')
  }
}