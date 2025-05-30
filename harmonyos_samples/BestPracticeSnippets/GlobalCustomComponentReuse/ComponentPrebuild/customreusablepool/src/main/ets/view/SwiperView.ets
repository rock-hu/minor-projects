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
import { NodeItem, NodePool } from '../utils/BuilderNodePool';

@Component
export struct SubSwiper {
  @State images: Resource[] = [];

  build() {
    Swiper() {
      ForEach(this.images, (res: Resource) => {
        Image(res)
          .width('100%')
          .height('100%')
      })
    }
    .loop(true)
    .width('100%')
    .height(200)
    .nestedScroll(SwiperNestedScrollMode.SELF_FIRST)
  }
}


// 瀑布流中的轮播图
@Builder
export function swiperBuilder(data: ESObject) {
  SubSwiper({
    images: data.data
  })
}

// 自定义组件复用池瀑布流子组件
@Component
export struct SwiperNode {
  @State type: string = '';
  @State images:Resource[]=[];
  @State builder: WrappedBuilder<ESObject> | null = null;
  private nodeItem: NodeItem = new NodeItem();

  aboutToAppear(): void {
    this.nodeItem = NodePool.getInstance().getNode(this.type, this.images, 0, this.builder!)!;
  }

  aboutToReuse(params: ESObject): void {
    this.nodeItem?.node?.reuse(params);
  }

  build() {
    NodeContainer(this.nodeItem)
  }
}

