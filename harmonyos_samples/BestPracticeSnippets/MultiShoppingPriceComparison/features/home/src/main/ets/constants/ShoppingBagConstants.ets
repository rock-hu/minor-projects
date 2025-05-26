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
import { Context } from "@kit.AbilityKit";

export class ShoppingBagConstants {
  /**
   * Product names.
   */
  static readonly PRODUCT_NAME_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.PRODUCT_NAME_LIST1')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_NAME_LIST2')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_NAME_LIST3')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_NAME_LIST1'))
  ];
  /**
   * Product types.
   */
  static readonly PRODUCT_TYPE_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.PRODUCT_TYPE_LIST1')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_TYPE_LIS2')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_TYPE_LIST3')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_TYPE_LIST1'))
  ];
  /**
   * Product images.
   */
  static readonly PRODUCT_IMAGE_LIST: Resource[] = [$r('app.media.shopping_card_1'),
    $r('app.media.shopping_card_2'), $r('app.media.shopping_card_3'), $r('app.media.shopping_card_1')];
  /**
   * Product preferential list.
   */
  static readonly PRODUCT_PREFERENTIALList_LIST = (context: Context): Array<Array<string>> => [
    [context.resourceManager.getStringSync($r('app.string.PRODUCT_PREFERENTIALList_LIST_MESSAGE1')),
      context.resourceManager.getStringSync($r('app.string.PRODUCT_PREFERENTIALList_LIST_MESSAGE2'))],
    [context.resourceManager.getStringSync($r('app.string.PRODUCT_PREFERENTIALList_LIST_MESSAGE3')),
      context.resourceManager.getStringSync($r('app.string.PRODUCT_PREFERENTIALList_LIST_MESSAGE4'))],
    [context.resourceManager.getStringSync($r('app.string.PRODUCT_PREFERENTIALList_LIST_MESSAGE1')),
      context.resourceManager.getStringSync($r('app.string.PRODUCT_PREFERENTIALList_LIST_MESSAGE4'))],
    [context.resourceManager.getStringSync($r('app.string.PRODUCT_PREFERENTIALList_LIST_MESSAGE1')),
      context.resourceManager.getStringSync($r('app.string.PRODUCT_PREFERENTIALList_LIST_MESSAGE2'))]];
  /**
   * Product price list.
   */
  static readonly PRODUCT_PRICE_LIST: number[] = [6199, 6999, 6099, 6199];
  /**
   * Product original price list.
   */
  static readonly PRODUCT_ORIGINAL_PRICE_LIST: number[] = [6999, -1, 6999, 6999];
  /**
   * Product present list.
   */
  static readonly PRODUCT_PRESENT_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRESENT_LIST_MESSAGE1')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRESENT_LIST_MESSAGE2')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRESENT_LIST_MESSAGE3')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_PRESENT_LIST_MESSAGE1'))
  ];
  /**
   * Product service list.
   */
  static readonly PRODUCT_SERVICE_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.PRODUCT_SERVICE_LIST_message')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_SERVICE_LIST_message')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_SERVICE_LIST_message')),
    context.resourceManager.getStringSync($r('app.string.PRODUCT_SERVICE_LIST_message'))
  ];
  /**
   * Preference details title.
   */
  static readonly PREFERENCE_DETAILS_TITLE = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE1')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE12')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE2')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE3')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE4'))
  ];
  /**
   * Preference details price.
   */
  static readonly PREFERENCE_DETAILS_PRICE: string[] = ['13999', '-800', '-200', '-1000', '12999'];
  /**
   * Shopping bag layout weight list.
   */
  static readonly SHOPPING_BAG_LAYOUT_WEIGHTS: number[] = [35, 24];
  /**
   * Shopping bag titles.
   */
  static readonly SHOPPING_BAG_TITLES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.SHOPPING_BAG_TITLES1')),
    context.resourceManager.getStringSync($r('app.string.SHOPPING_BAG_TITLES2')),
    context.resourceManager.getStringSync($r('app.string.SHOPPING_BAG_TITLES3'))];
  /**
   * Shopping discount title list.
   */
  static readonly SHOPPING_DISCOUNT_TITLES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE5')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE6')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE7')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE8')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE1')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE9'))
  ];
  /**
   * Shopping card foot title list.
   */
  static readonly SHOPPING_CARD_FOOT_TITLES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.SHOPPING_CARD_FOOT_TITLES1')),
    context.resourceManager.getStringSync($r('app.string.SHOPPING_CARD_FOOT_TITLES2')),
    '￥', '1299',
    context.resourceManager.getStringSync($r('app.string.SHOPPING_CARD_FOOT_TITLES3')),
    context.resourceManager.getStringSync($r('app.string.SHOPPING_CARD_FOOT_TITLES4'))];
  /**
   * Shopping card name list.
   */
  static readonly SHOPPING_CARD_NAMES = (context: Context): string[] => ['￥', '-', '1', '+',
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE10')),
    context.resourceManager.getStringSync($r('app.string.PREFERENCE_DETAILS_TITLE_MESSAGE11')),
  ];
}