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

import router from '@ohos.router';

class TextList {
  public id: number;
  public text: string;
  public url: string;

  constructor(id: number, text: string, url: string) {
    this.id = id;
    this.text = text;
    this.url = url;
  }
}

@Entry
@Component
struct IndexPage {
  scroll: Scroller = new Scroller()
  @State arr: TextList[] = [
    new TextList(1, 'Back', 'pages/Index'),
    new TextList(2, 'childrenMainSize', 'pages/List/childrenMainSize/ListItemGroupScenario'),
    new TextList(3, 'RepeatList1', 'pages/List/RepeatList/RepeatList1'),
    new TextList(4, 'AutoSort', 'pages/List/AutoSort/AutoSort'),
    new TextList(5, 'DeleteAndShift', 'pages/List/DeleteAndShift/DeleteAndShift'),
    new TextList(6, 'DragAndDropSorting', 'pages/List/DragAndDropSorting/DragAndDropSorting'),
    new TextList(7, 'FadingEdge', 'pages/List/FadingEdge/FadingEdge'),
    new TextList(8, 'LoopScrolling', 'pages/List/LoopScrolling/LoopScrolling'),
    new TextList(9, 'Sideslip', 'pages/List/Sideslip/Sideslip'),
    new TextList(10, 'MouseDragSlide', 'pages/List/MouseDragSlide'),
    new TextList(11, 'LayoutFromBottom', 'pages/List/LayoutFromBottom'),
    new TextList(12, 'CustomSlidingCurve', 'pages/List/CustomSlidingCurve'),
    new TextList(13, 'SlideHorizontallyOneSide', 'pages/List/SlideHorizontallyOneSide'),
    new TextList(14, 'FoldExpand', 'pages/List/FoldExpand'),
    new TextList(15, 'MultiSelectSliding', 'pages/List/MultiSelectSliding'),
    new TextList(16, '点击放大item', 'pages/List/DragAndDropSorting/ListItemGroup'),
    new TextList(17, 'OnWillStopDragging', 'pages/List/ListOnWillStopDragging/OnWillStopDragging'),
    new TextList(18, 'Table', 'pages/List/Table/TableDemo'),
  ]
  @State fontSize: Resource | undefined = undefined
  build() {
    Column() {
      Scroll(this.scroll) {
        Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
          ForEach(this.arr, (item: TextList) => {
            Button(item.text)
              .width(300)
              .height(50)
              .margin(10)
              .padding({ top: 5, bottom: 5 })
              .onClick(() => {
                router.pushUrl({ url: item.url })
              })
          }, (item: TextList) => JSON.stringify(item.id))
        }
        .width('100%')
      }
      .scrollBar(BarState.Off)
    }
  }
}