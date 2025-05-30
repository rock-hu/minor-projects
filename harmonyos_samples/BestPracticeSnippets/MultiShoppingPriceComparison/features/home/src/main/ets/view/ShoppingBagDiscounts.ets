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

import { BreakpointConstants, ResourceUtil, CommonConstants } from '@ohos/commons';
import { deviceInfo } from '@kit.BasicServicesKit';
import { ShoppingCardItem } from './ShoppingCardItem';
import { ShoppingBagListViewModel, ShoppingBagProduct, PreferenceDetails, PreferenceDetailsList }
  from '../viewmodel/ShoppingBagListViewModel';
import { ShoppingCardFoot } from './ShoppingCardFoot';
import { ShoppingBagConstants } from '../constants/ShoppingBagConstants';

@Component
export struct ShoppingBagDiscounts {
  @StorageLink('currentBreakpoint') currentBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('topRectHeight') topRectHeight: number = 0;
  private context: Context = this.getUIContext().getHostContext()!
  private ShoppingBagList: ShoppingBagProduct[]= new ShoppingBagListViewModel(this.context).getShoppingBagItemData();
  private PreferenceDetailsList: PreferenceDetails[]= new PreferenceDetailsList(this.context).getPreferenceDetailsList();
  private ShoppingBagProductItem: ShoppingBagProduct = this.ShoppingBagList[2];

  build() {
    Column() {
      Scroll() {
        Column() {
          Text(ShoppingBagConstants.SHOPPING_DISCOUNT_TITLES(this.context)[0])
            .fontSize($r('app.float.bag_discount_font_1'))
            .fontWeight(CommonConstants.FONT_WEIGHT_500)
            .lineHeight($r('app.float.bag_discount_line_1'))
          Text(ShoppingBagConstants.SHOPPING_DISCOUNT_TITLES(this.context)[1])
            .fontSize($r('app.float.bag_discount_font_2'))
            .lineHeight($r('app.float.bag_discount_line_2'))
            .fontWeight(FontWeight.Normal)
            .padding({
              top: $r('app.float.bag_discount_title_padding_top'),
              bottom: $r('app.float.bag_discount_title_padding_bottom')
            })

          Column() {
            ShoppingCardItem({
              shoppingBagProductItem: this.ShoppingBagProductItem,
              titleEllipsis: true
            })
            Row() {
              Text(ShoppingBagConstants.SHOPPING_DISCOUNT_TITLES(this.context)[2])
                .fontSize($r('app.float.bag_discount_font_3'))
                .fontWeight(CommonConstants.FONT_WEIGHT_500)
              Blank()
              Text(ShoppingBagConstants.SHOPPING_DISCOUNT_TITLES(this.context)[3])
                .fontSize($r('app.float.bag_discount_font_2'))
                .fontWeight(FontWeight.Normal)
            }
            .height($r('app.float.bag_discount_row_height'))
            .width(CommonConstants.FULL_PERCENT)
            .justifyContent(FlexAlign.Center)

            Row() {
              Text(ShoppingBagConstants.SHOPPING_DISCOUNT_TITLES(this.context)[4])
                .fontSize($r('app.float.bag_discount_font_3'))
                .fontWeight(CommonConstants.FONT_WEIGHT_500)
              Blank()
              Text(ShoppingBagConstants.SHOPPING_DISCOUNT_TITLES(this.context)[5])
                .fontSize($r('app.float.bag_discount_font_2'))
                .fontWeight(FontWeight.Normal)
              Image($r('app.media.ic_public_more_list'))
                .height(ResourceUtil.getCommonImgSize()[0])
                .width(ResourceUtil.getCommonImgSize()[0])
            }
            .justifyContent(FlexAlign.Center)
            .height($r('app.float.bag_discount_row_height'))
            .width(CommonConstants.FULL_PERCENT)
          }
          .backgroundColor(Color.White)
          .padding($r('app.float.bag_discount_padding'))
          .border({ radius: ResourceUtil.getCommonBorderRadius()[6] })
          .margin({ bottom: $r('app.float.bag_discount_margin') })

          Column() {
            ForEach(this.PreferenceDetailsList, (item: PreferenceDetails) => {
              Row() {
                Text(item.getTitle())
                  .fontSize($r('app.float.bag_discount_font_3'))
                  .fontWeight(FontWeight.Normal)
                Blank()
                Text(item.getPrice())
                  .fontSize($r('app.float.bag_discount_font_2'))
                  .fontColor('#CF0A2C')
              }
              .justifyContent(FlexAlign.Center)
              .height($r('app.float.bag_discount_row_height'))
              .width(CommonConstants.FULL_PERCENT)
            }, (item: PreferenceDetails, index: number) => JSON.stringify(item) + index)
          }
          .backgroundColor(Color.White)
          .padding($r('app.float.bag_discount_padding'))
          .border({ radius: ResourceUtil.getCommonBorderRadius()[6] })
        }
        .width(CommonConstants.FULL_PERCENT)
        .alignItems(HorizontalAlign.Start)
        .justifyContent(FlexAlign.Start)
        .padding({
          left: ResourceUtil.getPageColPadding().getValue(this.currentBreakpoint),
          right: ResourceUtil.getPageColPadding().getValue(this.currentBreakpoint),
          top: deviceInfo.deviceType === CommonConstants.DEVICE_TYPES[0] ?
            $r('app.float.bag_discount_col_padding_bottom') : this.topRectHeight,
        })

      }
      .width(CommonConstants.FULL_PERCENT)
      .scrollBarWidth(0)
      .layoutWeight(1)
      .backgroundColor(ResourceUtil.getCommonBackgroundColor()[0])

      ShoppingCardFoot({ isMoreDetail: true })
        .margin({ bottom: deviceInfo.deviceType !== CommonConstants.DEVICE_TYPES[0] ?
        CommonConstants.BOTTOM_RECT_HEIGHT : 0 })
    }
    .backgroundColor($r('app.color.bag_discount_background'))
    .layoutWeight(ShoppingBagConstants.SHOPPING_BAG_LAYOUT_WEIGHTS[1])
    .height(CommonConstants.FULL_PERCENT)
    .visibility(this.currentBreakpoint === BreakpointConstants.BREAKPOINT_LG ? Visibility.Visible : Visibility.None)
  }
}