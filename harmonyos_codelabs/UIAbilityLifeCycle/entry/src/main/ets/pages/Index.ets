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

// [Start LifeCycle_Index_List]
import { common } from '@kit.AbilityKit';
import { ModelData } from '../model/ModelData';

// [Start LifeCycle_Index_Title]
// src/main/ets/pages/Index.ets
@Entry
@Component
struct Index {
  // [StartExclude LifeCycle_Index_Title]
  @StorageProp('modelDataList') modelDataList: ModelData[] = [];
  private context = this.getUIContext().getHostContext() as common.UIAbilityContext;

  aboutToAppear(): void {
    this.context.eventHub.on('ability', (modelDataList: ModelData[]) => {
      this.modelDataList = modelDataList;
    })
  }
  // [EndExclude LifeCycle_Index_Title]

  // [StartExclude LifeCycle_Index_List]
  @Builder
  titleBuilder() {
    Column() {
      Text($r('app.string.home_title'))
        .fontSize(30)
        .lineHeight(40)
        .fontWeight(700)
        .width('100%')
        .fontColor('rgba(0, 0, 0, 0.9)')
    }
    .justifyContent(FlexAlign.Center)
    .margin({
      left: 16,
      right: 16,
      top: 64,
      bottom: 8
    })
    .height(36)
  }
  // [EndExclude LifeCycle_Index_List]

  build() {
    Navigation() {
      // [StartExclude LifeCycle_Index_Title]
      // [Start LifeCycle_Index_Content]
      Column() {
        // [StartExclude LifeCycle_Index_List]
        Row() {
          Text($r('app.string.home_log'))
            .width(158)
            .height(45)
            .fontSize(14)
            .fontWeight(500)
            .lineHeight(19)
            .fontColor('rgba(0, 0, 0, 0.6)')
        }
        .width('100%')
        .height(56)
        .justifyContent(FlexAlign.Start)
        .padding({ left: 12 })
        // [EndExclude LifeCycle_Index_List]

        List({ space: 5 }) {
          ForEach(this.modelDataList, (modelData: ModelData) => {
            ListItem() {
              Row({ space: 10 }) {
                Text(modelData.time + ':' + modelData.title)
                  .fontSize(16)
                  .fontWeight(400)
                  .lineHeight(21)
                  .fontColor('rgba(0, 0, 0, 0.6)')
                  .textOverflow({ overflow: TextOverflow.Ellipsis })
              }
              .width('100%')
            }
          }, (modelData: ModelData, index: number) => JSON.stringify(modelData) + index)
        }
        .width('100%')
        .layoutWeight(1)
        .backgroundColor(Color.White)
        .borderRadius(16)
        .padding({
          left: 16,
          right: 16,
          top: 8,
          bottom: 8
        })
      }
      .height('100%')
      .width('100%')
      .padding(16)
      // [End LifeCycle_Index_Content]
      // [EndExclude LifeCycle_Index_Title]
    }
    // [StartExclude LifeCycle_Index_List]
    .title({ builder: this.titleBuilder, height: 112 })
    .navBarWidth(300)
    .backgroundColor('rgb(241, 243, 245)')
    // [EndExclude LifeCycle_Index_List]
  }
}
// [End LifeCycle_Index_Title]
// [Start LifeCycle_Index_List]