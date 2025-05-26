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

export class LiveConstants {
  /**
   * Live product image list.
   */
  static readonly LIVE_PRODUCT_IMAGE_LIST: Resource[] = [$r('app.media.live_product_1'), $r('app.media.live_product_2'),
    $r('app.media.live_product_3'), $r('app.media.live_product_4'), $r('app.media.live_product_5'),
    $r('app.media.live_product_6'), $r('app.media.live_product_7'), $r('app.media.live_product_8')];
  /**
   * Live product name list.
   */
  static readonly LIVE_PRODUCT_NAME_LIST: string[] = ['HUAWEI Mate 60 Pro', 'XXXXXXXXXXX', 'XXXXXXXXXXX', 'XXXXXXXXXXX',
    'XXXXXXXXXXX', 'XXXXXXXXXXX', 'XXXXXXXXXXX', 'XXXXXXXXXXX'];
  /**
   * Live product price list.
   */
  static readonly LIVE_PRODUCT_PRICE_LIST: string[] =
    ['￥ 6999', 'XXXX', 'XXXX', 'XXXX', 'XXXX', 'XXXX', 'XXXX', 'XXXX'];
  /**
   * Shop car product image list.
   */
  static readonly SHOP_CAR_PRODUCT_IMAGE_LIST: Resource[] = [$r('app.media.live_bag_product_1'),
    $r('app.media.live_bag_product_2'), $r('app.media.live_bag_product_3'), $r('app.media.live_bag_product_4')];
  /**
   * Shop car product name list.
   */
  static readonly SHOP_CAR_PRODUCT_NAME_LIST: string[] = ['HUAWEI Mate 60 Pro', 'Mate Xs 2', 'MateBook E Go',
    'HUAWEI Mate 50'];
  /**
   * Shop car product remark list.
   */
  static readonly SHOP_CAR_PRODUCT_REMARK_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.OP_CAR_PRODUCT_REMARK_LIST1')),
    context.resourceManager.getStringSync($r('app.string.OP_CAR_PRODUCT_REMARK_LIST2')),
    context.resourceManager.getStringSync($r('app.string.OP_CAR_PRODUCT_REMARK_LIST3')),
    context.resourceManager.getStringSync($r('app.string.OP_CAR_PRODUCT_REMARK_LIST1'))];
  /**
   * Shop car product discounts list.
   */
  static readonly SHOP_CAR_PRODUCT_DISCOUNTS_LIST = (context: Context): string[][] => [
    [context.resourceManager.getStringSync($r('app.string.Gifted_points')),
      context.resourceManager.getStringSync($r('app.string.flagship_machine'))],
    [context.resourceManager.getStringSync($r('app.string.flagship_machine'))],
    [context.resourceManager.getStringSync($r('app.string.cheaper'))],
    [context.resourceManager.getStringSync($r('app.string.Gifted_points')),
      context.resourceManager.getStringSync($r('app.string.flagship_machine'))]];
  /**
   * Shop car product price list.
   */
  static readonly SHOP_CAR_PRODUCT_PRICE_LIST: number[] = [6999, 9999, 4899, 4999];
  /**
   * Shop car icon resources.
   */
  static readonly SHOP_CAR_ICON_RESOURCES: Resource[] = [$r('app.media.icon_bag'), $r('app.media.icon_search'),
    $r('app.media.icon_order')];
  /**
   * Shop car icon names.
   */
  static readonly SHOP_CAR_ICON_NAMES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.Shopping_bag')),
    context.resourceManager.getStringSync($r('app.string.search')),
    context.resourceManager.getStringSync($r('app.string.Order'))];
  /**
   * Shop car tab list.
   */
  static readonly SHOP_CAR_TABS = (context: Context): string[] => {
    return [
      context.resourceManager.getStringSync($r('app.string.all')),
      context.resourceManager.getStringSync($r('app.string.phone')),
      context.resourceManager.getStringSync($r('app.string.mac')),
      context.resourceManager.getStringSync($r('app.string.PC')),
      context.resourceManager.getStringSync($r('app.string.watch')),
      context.resourceManager.getStringSync($r('app.string.earphone'))]
  };
  /**
   * Live Comments.
   */
  static readonly LIVE_COMMENTS_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.LIVE_COMMENTS_LIST1')),
    context.resourceManager.getStringSync($r('app.string.LIVE_COMMENTS_LIST2')),
    context.resourceManager.getStringSync($r('app.string.LIVE_COMMENTS_LIST3'))];
  /**
   * Comments layout weight.
   */
  static readonly COMMENTS_WIDTH: string[] = ['228vp', '400vp', '290vp']
  /**
   * Shopping list interval.
   */
  static readonly SHOPPING_LIST_SPACE: string[] = ['12vp', '10vp'];
  /**
   * Currency symbol.
   */
  static readonly CURRENCY_SYMBOL: string = "￥ ";
  /**
   * Eight.
   */
  static readonly EIGHT: number = 8;
  /**
   * Ninety percent.
   */
  static readonly NINETY_PERCENT: string = '90%';
  /**
   * Pay button text.
   */
  static readonly PAY_BUTTON_TEXT =
    (context: Context): string => context.resourceManager.getStringSync($r('app.string.buy_now'));
  /**
   * X component params.
   */
  static readonly X_COMPONENT_PARAMS: string[] = ['live', 'surface'];
  /**
   * X component aspect ratio.
   */
  static readonly X_COMPONENT_ASPECT_RATIO: number = 0.56;
}