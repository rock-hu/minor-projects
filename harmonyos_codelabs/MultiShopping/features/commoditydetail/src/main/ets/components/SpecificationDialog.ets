/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { Specification, CounterProduct, Commodity, KV, GridConstants, StyleConstants } from '@ohos/common';
import { CapsuleGroupButton } from './CapsuleGroupButton';
import { ColorChange, FinishType } from '../viewmodel/TypeModel';
import { CommodityConstants } from '../constants/CommodityConstants';
import { ProductSpecification } from '@ohos/common/src/main/ets/viewmodel/ProductModel';

@CustomDialog
export struct SpecificationDialog {
  @State colorChange: ColorChange = {
    Color: '',
    Version: '',
    Network: '',
    Type: ''
  };
  @Link data: Commodity;
  @Link count: number;
  @Link selectTags: ProductSpecification[];
  @Link productOptions: string;
  controller?: CustomDialogController;
  private onFinish?: (type: FinishType, count: number, selectKeys: ProductSpecification[]) => void;
  aboutToAppear(){
    this.selectTags.forEach((item) => {
      switch (this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.name)) {
        case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_color')):
          this.colorChange.Color = this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.value);
          break;
        case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_version')):
          this.colorChange.Version = this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.value);
          break;
        case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_network_standard')):
          this.colorChange.Network = this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.value);
          break;
        case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_type')):
          this.colorChange.Type = this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.value);
          break;
        default:
          break;
      }
    })
  }

  build() {
    GridRow({ columns: { sm: GridConstants.COLUMN_FOUR, md: GridConstants.COLUMN_EIGHT,
      lg: GridConstants.COLUMN_TWELVE }, gutter: GridConstants.GUTTER_TWELVE }) {
      GridCol({ span: { sm: GridConstants.SPAN_FOUR, md: GridConstants.SPAN_EIGHT, lg: GridConstants.SPAN_EIGHT },
        offset: { lg: GridConstants.OFFSET_TWO } }) {
        Column() {
          Image($r('app.media.ic_normal'))
            .width($r('app.float.dialog_normal_image_width'))
            .height($r('app.float.vp_twenty_four'))
            .objectFit(ImageFit.Contain)
            .onClick(() => {
              this.controller?.close();
              if (this.onFinish) {
                this.onFinish(FinishType.CANCEL, this.count, this.selectTags);
              }
            })
          Row() {
            Image(this.data.images && $rawfile(this.data.images[0]))
              .width($r('app.float.dialog_commodity_image_size'))
              .height($r('app.float.dialog_commodity_image_size'))
              .objectFit(ImageFit.Cover)
              .margin({
                left: $r('app.float.vp_sixteen'),
                right: $r('app.float.vp_sixteen')
              })
            Column() {
              Text() {
                Span($r('app.string.rmb'))
                  .fontSize($r('app.float.middle_font_size'))
                  .fontColor($r('app.color.focus_color'))
                Span(`${this.data.price}`)
                  .fontSize($r('app.float.bigger_font_size'))
                  .fontColor($r('app.color.focus_color'))
              }
              .margin({ bottom: $r('app.float.vp_twelve') })

              Text(`${this.getUIContext().getHostContext()!.resourceManager.getStringSync(CommodityConstants.SPECIAL_CHOOSE)} ：${this.productOptions} ${this.count ? `X${this.count}` : ''}`)
                .fontSize($r('app.float.smaller_font_size'))
                .fontColor(Color.Black)
                .maxLines(CommodityConstants.MAX_LINE)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
            }
            .layoutWeight(StyleConstants.LAYOUT_WEIGHT)
            .alignItems(HorizontalAlign.Start)
          }
          .margin({
            top: $r('app.float.vp_twenty_four'),
            bottom: $r('app.float.vp_twenty_four'),
            left: $r('app.float.vp_twelve'),
            right: $r('app.float.vp_twelve')
          })

          Scroll() {
            Column() {
              ForEach(this.data.specifications || [], (item: Specification) => {
                this.Specification(item)
              }, (item: Specification) => this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.id))
              Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
                Text($r('app.string.quantity'))
                  .fontSize($r('app.float.small_font_size'))
                  .fontColor($r('app.color.sixty_alpha_black'))
                CounterProduct({
                  count: this.count,
                  onNumberChange: (num: number) => this.count = num
                })
              }
              .margin({ right: $r('app.float.vp_twenty_four') })
            }
          }
          .margin({ left: $r('app.float.vp_sixteen') })

          this.ButtonGroup()
        }
        .border({
          radius: {
            topRight: $r('app.float.dialog_radius'),
            topLeft: $r('app.float.dialog_radius')
          }
        })
        .backgroundColor($r('app.color.page_background'))
        .width(StyleConstants.FULL_WIDTH)
      }
    }
  }

  @Builder ButtonGroup() {
    Flex() {
      CapsuleGroupButton({
        configs: [{
          text: $r('app.string.insert_cart'),
          onClick: () => {
            this.controller?.close();
            if (this.onFinish) {
              this.onFinish(FinishType.JOIN_SHOPPING_CART, this.count, this.selectTags);
            }
          }
        }, {
          text: $r('app.string.buy_now'),
          onClick: () => {
            this.controller?.close();
            if (this.onFinish) {
              this.onFinish(FinishType.BUY_NOW, this.count, this.selectTags);
            }
          }
        }]
      })
    }
    .width(StyleConstants.FULL_WIDTH)
    .height($r('app.float.vp_fifty_six'))
    .padding($r('app.float.vp_twelve'))
  }
  colorChanges(key: string): string {
    let temp: string = '';
    switch (key) {
      case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_color')):
        temp = this.colorChange.Color;
        break;
      case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_version')):
        temp = this.colorChange.Version;
        break;
      case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_network_standard')):
        temp = this.colorChange.Network;
        break;
      case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_type')):
        temp = this.colorChange.Type;
        break;
      default:
        break;
    }
    return temp;
  }

  @Builder Specification(payload: Specification) {
    Column() {
      Text(payload.title)
        .fontSize($r('app.float.small_font_size'))
        .margin({ bottom: $r('app.float.vp_twelve') })
        .fontColor($r('app.color.sixty_alpha_black'))
      Flex({ wrap: FlexWrap.Wrap }) {
        ForEach(payload.data, (item: KV) => {
          Text(item.key)
            .fontSize($r('app.float.smaller_font_size'))
            .fontColor(this.colorChanges(this.getUIContext().getHostContext()!.resourceManager.getStringSync(payload.id)) === this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.value) ? $r('app.color.focus_color') : Color.Black)
            .height($r('app.float.vp_twenty_four'))
            .padding({
              top: $r('app.float.vp_six'),
              bottom: $r('app.float.vp_six'),
              left: $r('app.float.vp_ten'),
              right: $r('app.float.vp_ten')
            })
            .backgroundColor(this.colorChanges(this.getUIContext().getHostContext()!.resourceManager.getStringSync(payload.id)) === this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.value)  ?
              $r('app.color.dialog_select_tag') : $r('app.color.five_alpha_black'))
            .margin({ bottom: $r('app.float.vp_ten'), right: $r('app.float.vp_twelve') })
            .borderRadius($r('app.float.vp_twelve'))
            .onClick(():void => this.onTagSelect(item, this.getUIContext().getHostContext()!.resourceManager.getStringSync(payload.id)))
        }, (item: KV) => JSON.stringify(item.value))
      }
    }
    .alignItems(HorizontalAlign.Start)
    .width(StyleConstants.FULL_WIDTH)
    .margin({ bottom: $r('app.float.vp_fourteen') })
  }

  onTagSelect(tag: KV, parentId: string) {
    this.selectTags.forEach((item, index) => {
      if (parentId === this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.name)) {
        switch (parentId) {
          case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_color')):
            this.colorChange.Color = this.getUIContext().getHostContext()!.resourceManager.getStringSync(tag.value);
            break;
          case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_version')):
            this.colorChange.Version = this.getUIContext().getHostContext()!.resourceManager.getStringSync(tag.value);
            break;
          case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_network_standard')):
            this.colorChange.Network = this.getUIContext().getHostContext()!.resourceManager.getStringSync(tag.value);
            break;
          case this.getUIContext().getHostContext()!.resourceManager.getStringSync($r('app.string.case_type')):
            this.colorChange.Type = this.getUIContext().getHostContext()!.resourceManager.getStringSync(tag.value);
            break;
          default:
            break;
        }
        let data: ProductSpecification = {name:item.name,value:tag.value}
        this.selectTags.splice(index,1,data);
      }
    })
    this.onSelectKeysChange()
  }

  onSelectKeysChange(){
    this.productOptions = '';
    this.selectTags.forEach((item)=>{
      this.productOptions += this.getUIContext().getHostContext()!.resourceManager.getStringSync(item.value)+' ';
    })
  }
}