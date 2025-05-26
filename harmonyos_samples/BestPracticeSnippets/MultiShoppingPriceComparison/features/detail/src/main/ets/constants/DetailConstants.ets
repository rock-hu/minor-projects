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

export class DetailConstants {
  /**
   * Living.
   */
  static readonly LIVING = (context: Context): string => context.resourceManager.getStringSync($r('app.string.living'));

  /**
   * Product detail resources.
   */
  static readonly PRODUCT_DETAIL_RESOURCES: Resource[] = [$r('app.media.product_detail_1'),
    $r('app.media.product_detail_2'), $r('app.media.product_detail_2'), $r('app.media.product_detail_2'),
    $r('app.media.product_detail_3')];

  /**
   * Product small resources.
   */
  static readonly PRODUCT_SMALL_RESOURCES: Resource[] = [$r('app.media.product_small_1'),
    $r('app.media.product_small_2'), $r('app.media.product_small_2'), $r('app.media.product_small_2'),
    $r('app.media.product_small_3')];

  /**
   * Gradient color list.
   */
  static readonly GRADIENT_COLORS: string[] = ['#FF6354', '#CF0A2C', '#FAE8E0', '#FFC9C9'];

  /**
   * Product color selection.
   */
  static readonly PRODUCT_COLOR_SELECTION: Resource[] = [$r('app.media.product_small_1'),
    $r('app.media.product_small_2'), $r('app.media.product_small_3')];

  /**
   * Recommended config products.
   */
  static readonly RECOMMENDED_CONFIG_PRODUCTS: Resource[] = [$r('app.media.recommended_config_1'),
    $r('app.media.recommended_config_2'), $r('app.media.recommended_config_3'), $r('app.media.recommended_config_4')];

  /**
   * Param string list.
   */
  static readonly PARAM_STRING_LIST: string[] = ['shoppingBag', 'customerService'];

  /**
   * Customer service contents.
   */
  static readonly CUSTOMER_SERVICE_CONTENTS = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.store')),
    '￥12999', '￥13999',
    context.resourceManager.getStringSync($r('app.string.buy')),
    context.resourceManager.getStringSync($r('app.string.CUSTOMER_SERVICE_CONTENTS1')),
    context.resourceManager.getStringSync($r('app.string.CUSTOMER_SERVICE_CONTENTS2')),
    context.resourceManager.getStringSync($r('app.string.CUSTOMER_SERVICE_CONTENTS3')),
    context.resourceManager.getStringSync($r('app.string.CUSTOMER_SERVICE_CONTENTS4'))];

  /**
   * Product color list.
   */
  static readonly COLOR_SELECT_NAME_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.COLOR_SELECT_NAME_LIST1')),
    context.resourceManager.getStringSync($r('app.string.COLOR_SELECT_NAME_LIST2')),
    context.resourceManager.getStringSync($r('app.string.COLOR_SELECT_NAME_LIST3')),
    context.resourceManager.getStringSync($r('app.string.COLOR_SELECT_NAME_LIST4'))];

  /**
   * Product name list.
   */
  static readonly CAPACITY_SELECT_NAME_LIST: string[] = ['12GB+512GB', '12GB+1T'];

  /**
   * Product type list.
   */
  static readonly TYPE_SELECT_NAME_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.Official_standard'))];

  /**
   * Product select type list.
   */
  static readonly SELECT_NAME_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.SELECT_NAME_LIST1')),
    context.resourceManager.getStringSync($r('app.string.SELECT_NAME_LIST2')),
    context.resourceManager.getStringSync($r('app.string.SELECT_NAME_LIST3'))];

  /**
   * Product image list.
   */
  static readonly COLOR_SELECT_IMAGE_LIST: Resource[] = [$r('app.media.pay_product_1'),
    $r('app.media.pay_product_2'), $r('app.media.pay_product_3'), $r('app.media.pay_product_4')];

  /**
   * Button names.
   */
  static readonly BUTTON_NAMES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.add_cart')),
    context.resourceManager.getStringSync($r('app.string.buy'))];

  /**
   * Pay address and contact.
   */
  static readonly PAY_INFO = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.pay_info1')),
    context.resourceManager.getStringSync($r('app.string.pay_info2')),
    context.resourceManager.getStringSync($r('app.string.pay_info3')),
    context.resourceManager.getStringSync($r('app.string.pay_info4'))];

  /**
   * Product config list.
   */
  static readonly PRODUCT_CONFIG_NAMES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.PRODUCT_CONFIG_NAMES1')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_CONFIG_NAMES2')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_CONFIG_NAMES3')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_CONFIG_NAMES4')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_CONFIG_NAMES5')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_CONFIG_NAMES6')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_CONFIG_NAMES7')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_CONFIG_NAMES8'))];

  /**
   * User comment list.
   */
  static readonly USER_COMMENT_NAMES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.USER_COMMENT_NAMES1')),
    context.resourceManager.getStringSync($r('app.string.USER_COMMENT_NAMES2')),
    context.resourceManager.getStringSync($r('app.string.USER_COMMENT_NAMES3')),
    context.resourceManager.getStringSync($r('app.string.USER_COMMENT_NAMES4')),
    context.resourceManager.getStringSync($r('app.string.USER_COMMENT_NAMES5')),
    context.resourceManager.getStringSync($r('app.string.USER_COMMENT_NAMES6')),
    context.resourceManager.getStringSync($r('app.string.USER_COMMENT_NAMES7')),
    context.resourceManager.getStringSync($r('app.string.USER_COMMENT_NAMES8'))];

  /**
   * List space.
   */
  static readonly PRODUCT_CONFIG_LIST_SPACE: number = 12;

  /**
   * Y position of live row.
   */
  static readonly LIVE_ROW_Y_POSITION: number = 12;

  /**
   * Live stack aspect ratio.
   */
  static readonly LIVE_STACK_ASPECT_RATIO: number[] = [1.2, 2.2];

  /**
   * Picture number.
   */
  static readonly PICTURE_INDICATOR = (context: Context): string[] => ['/5', '3D',
    context.resourceManager.getStringSync($r('app.string.compare'))];

  /**
   * Picture list space list.
   */
  static readonly PICTURE_LIST_SPACES: string[] = ['8vp', '12vp', '16vp'];

  /**
   * Product discount name list.
   */
  static readonly PRODUCT_DISCOUNT_NAMES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.PRODUCT_DISCOUNT_NAMES1')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_DISCOUNT_NAMES2')),
    'Mate X5:',
    context.resourceManager.getStringSync($r('app.string.PRODUCT_DISCOUNT_NAMES3'))];

  /**
   * Product price name list.
   */
  static readonly PRODUCT_PRICE_NAMES = (context: Context): string[] => ['￥', '12999', '￥13999',
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRICE_NAMES1')),
    '07', ':', '14', ':', '50',
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRICE_NAMES2')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRICE_NAMES3')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRICE_NAMES4')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRICE_NAMES5')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRICE_NAMES6')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRICE_NAMES7')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRICE_NAMES8'))];

  /**
   * 80 percent.
   */
  static readonly EIGHTY_PERCENT: string = '80%';

  /**
   * Comments list.
   */
  static readonly COMMENTS_LIST: number[] = [1, 2];

  /**
   * Navigation id.
   */
  static readonly NAVIGATION_ID: string = 'navId';
}