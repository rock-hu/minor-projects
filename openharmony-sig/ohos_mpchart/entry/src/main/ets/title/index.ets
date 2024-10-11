/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import { ComponentContent } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';

@Entry
@ComponentV2
export default struct ChartTitle {
  @Param model: ChartTitleModel = new ChartTitleModel()
  @Event $model: (model: ChartTitleModel) => void = (model: ChartTitleModel) => {};

  build() {
    Column() {
      Row() {
        if(this.model.canBack){
          Image($r('app.media.back'))
            .width(50).height(30)
            .onClick(()=>{
              router.back()
            })
        }
        Text(this.model.title).fontSize(21).fontColor(Color.White).fontWeight(600).layoutWeight(1).textOverflow({overflow:TextOverflow.Ellipsis}).maxLines(1)
        if (this.model.isShowMenu && !!this.model.menuItemArr && this.model.menuItemArr.length > 0) {
          Column() {
            Image($r('app.media.more'))
              .width(28).height(30)
          }.width(50).alignItems(HorizontalAlign.End)
          .onClick(() => {
            let uiContext = this.getUIContext();
            let promptAction = uiContext.getPromptAction();
            let contentNode = new ComponentContent(uiContext, wrapBuilder(ChartMenu), new Params(this.model!!, (index: number) => {
                this.model.setIndex(index);
                this.$model(this.model);
                promptAction.closeCustomDialog(contentNode);
              }))
            try {
              promptAction.openCustomDialog(contentNode);
            } catch (error) {
              let message = (error as BusinessError).message;
              let code = (error as BusinessError).code;
              console.error(`OpenCustomDialog args error code is ${code}, message is ${message}`);
            }
          })
        }
      }.backgroundColor(Color.Black).padding(12).width('100%').height(this.model.height)
    }.zIndex(10)
  }
}

@ObservedV2
export class ChartTitleModel {
  @Trace itemStr: string = ''
  @Trace index: number = -1;
  @Trace menuItemArr: Array<string> = [];
  @Trace title: string = 'title'
  @Trace isShowMenu: boolean = true
  @Trace height: number = 54
  @Trace canBack: boolean = true;

  setIndex(index: number): ChartTitleModel {
    this.index = index
    return this
  }

  getIndex(): number {
    return this.index
  }

  setItemStr(itemStr: string): ChartTitleModel {
    this.itemStr = itemStr
    return this
  }

  getCanBack(): boolean{
    return this.canBack;
  }

  setCanBack(canBack: boolean){
    this.canBack = canBack;
  }
}

interface MenuItemValue {
  i: number;
  data: string
}

@ObservedV2
class Params {
  @Trace model: ChartTitleModel = new ChartTitleModel();
  @Trace callBack: (index: number) => void;

  constructor(model: ChartTitleModel, callBack: (index: number) => void) {
    this.model = model;
    this.callBack = callBack;
  }
}

@Builder
function ChartMenu(params: Params) {
  Row() {
    Blank().layoutWeight(1);
    Column() {
      ForEach(params.model.menuItemArr.map((item: string, index: number) => {
        let value: MenuItemValue = { i: index, data: item }
        return value;
      }),
        (item: MenuItemValue) => {
          Text(item.data)
            .fontSize(21)
            .padding(16)
            .fontColor(Color.Black)
            .padding({
              top: 10,
              right: 20,
              bottom: 10,
              left: 20
            })
            .onClick(() => {
              params.callBack(item.i);
            })
        }, (item: MenuItemValue) => JSON.stringify(item))

    }.backgroundColor(Color.White).zIndex(1)
    .alignItems(HorizontalAlign.Start)
  }.width('100%').zIndex(10)
}



