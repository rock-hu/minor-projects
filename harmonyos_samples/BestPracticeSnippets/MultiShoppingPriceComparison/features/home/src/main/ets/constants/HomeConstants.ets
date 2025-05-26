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

export class HomeConstants {
  /**
   * Footer tab topics.
   */
  static readonly FOOTER_TOPIC_LIST = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.FOOTER_TOPIC_LIST1')),
    context.resourceManager.getStringSync($r('app.string.FOOTER_TOPIC_LIST2')),
    context.resourceManager.getStringSync($r('app.string.FOOTER_TOPIC_LIST3')),
    context.resourceManager.getStringSync($r('app.string.FOOTER_TOPIC_LIST4')),
    context.resourceManager.getStringSync($r('app.string.FOOTER_TOPIC_LIST5'))];
  /**
   * Footer topic icons.
   */
  static readonly FOOTER_TOPIC_ICONS: Resource[] = [$r('app.media.tab_home'), $r('app.media.tab_classification'),
    $r('app.media.tab_discovery'), $r('app.media.tab_shopping_bag'), $r('app.media.tab_mine')];
  /**
   * Footer topic icons.
   */
  static readonly FOOTER_TOPIC_ICONS_SELECTED: Resource[] = [$r('app.media.tab_home_selected'),
    $r('app.media.tab_classification_selected'), $r('app.media.tab_discovery_selected'),
    $r('app.media.tab_shopping_bag_selected'), $r('app.media.tab_mine_selected')];
  /**
   * Top bar text list.
   */
  static readonly TOP_BAR_TEXTS = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.TOP_BAR_TEXTS1')),
    context.resourceManager.getStringSync($r('app.string.TOP_BAR_TEXTS2')),
    context.resourceManager.getStringSync($r('app.string.TOP_BAR_TEXTS3')),
    context.resourceManager.getStringSync($r('app.string.TOP_BAR_TEXTS4'))];
  /**
   * Search placeholder.
   */
  static readonly SEARCH_PLACEHOLDER = (context: Context): string =>
    context.resourceManager.getStringSync($r('app.string.searching'));
  /**
   * Recommended product list.
   */
  static readonly RECOMMENDED_PRODUCTS: Resource[] = [$r('app.media.recommended_product_1'),
    $r('app.media.recommended_product_2'), $r('app.media.recommended_product_3'),
    $r('app.media.recommended_product_4')];
  /**
   * Recommended product is living list.
   */
  static readonly RECOMMENDED_PRODUCT_LIVING: boolean[] = [true, false, true, false];
  /**
   * Good categories.
   */
  static readonly GOOD_CATEGORIES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES1')),
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES2')),
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES3')),
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES4')),
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES5')),
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES6')),
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES7')),
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES8')),
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES9')),
    context.resourceManager.getStringSync($r('app.string.GOOD_CATEGORIES10'))];
  /**
   * Good category images.
   */
  static readonly GOOD_CATEGORY_IMG: Resource[] = [$r('app.media.categories_1'), $r('app.media.categories_2'),
    $r('app.media.categories_3'), $r('app.media.categories_4'), $r('app.media.categories_5'),
    $r('app.media.categories_6'), $r('app.media.categories_7'), $r('app.media.categories_8'),
    $r('app.media.categories_9'), $r('app.media.categories_10')];
  /**
   * Category list space list.
   */
  static readonly CATEGORY_LIST_SPACES: string[] = ['0', '36vp', '72vp'];
  /**
   * Category divider stroke width.
   */
  static readonly CATEGORY_DIVIDER_STROKE_WIDTH: string = '4vp';
  /**
   * Category list item width list.
   */
  static readonly CATEGORY_LIST_ITEM_WIDTHS: string[] = ['20%', '64vp', '68vp'];
  /**
   * Recommended product item space list.
   */
  static readonly RECOMMENDED_PRODUCT_ITEM_SPACES: string[] = ['16vp', '12vp'];
  /**
   * Recommended product view margin list.
   */
  static readonly RECOMMENDED_PRODUCT_VIEW_SPACES: number[] = [12, 16, 24, 32];
  /**
   * Welfare products.
   */
  static readonly WELFARE_PRODUCTS: string[] = ['1299', '899', '599', '2499', '6988', '1799', '988', '6999'];
  /**
   * Welfare product images.
   */
  static readonly WELFARE_PRODUCT_IMG: Resource[] = [$r('app.media.welfare_product_1'),
    $r('app.media.welfare_product_2'), $r('app.media.welfare_product_3'), $r('app.media.welfare_product_4'),
    $r('app.media.welfare_product_5'), $r('app.media.welfare_product_6'), $r('app.media.welfare_product_7'),
    $r('app.media.welfare_product_8')];
  /**
   * Price unit.
   */
  static readonly PRICE_UNIT = (context: Context): string[] => ['￥',
    context.resourceManager.getStringSync($r('app.string.PRICE_UNIT'))];
  /**
   * Home title list.
   */
  static readonly HOME_TITLES = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.HOME_TITLES1')),
    context.resourceManager.getStringSync($r('app.string.HOME_TITLES2')),
    context.resourceManager.getStringSync($r('app.string.HOME_TITLES3'))];
  /**
   * Home button text list.
   */
  static readonly BUTTON_TEXTS = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.BUTTON_TEXTS1')),
    context.resourceManager.getStringSync($r('app.string.BUTTON_TEXTS2'))];
  /**
   * Welfare tip text.
   */
  static readonly TIP_TEXTS  = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.TIP_TEXTS1')),
    context.resourceManager.getStringSync($r('app.string.TIP_TEXTS2')),
    context.resourceManager.getStringSync($r('app.string.TIP_TEXTS3'))];
  /**
   * Gradient color list.
   */
  static readonly GRADIENT_COLORS: string[] = ['#4DEDAA68', '#33FFE9C1', '#FF6354', '#EA1559'];
  /**
   * Section product image list.
   */
  static readonly SECTION_PRODUCT_IMAGES: Resource[] = [$r('app.media.section_product_1'),
    $r('app.media.section_product_2'), $r('app.media.section_product_3'), $r('app.media.section_product_4'),
    $r('app.media.section_product_5')];
  /**
   * Section products title list.
   */
  static readonly SECTION_PRODUCTS_TITLES = (context: Context): string[][] => [
    [context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_TITLES1'))],
    ['HUAWEI Mate 50',
      context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_TITLES2'))],
    ['Sound X NEW'], ['HUAWEI MatePad Air'], ['HUAWEI MateBook 14s']];
  /**
   * Section products info list.
   */
  static readonly SECTION_PRODUCTS_INFO = (context: Context): string[] => ['',
    context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_INFO1')), '',
    context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_INFO2')),
    context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_INFO3'))];
  /**
   * Section products price list.
   */
  static readonly SECTION_PRODUCTS_PRICES: string[] = ['￥10999', '￥4999', '￥2499', '￥2599', '￥8399'];
  /**
   * Section products feature list.
   */
  static readonly SECTION_PRODUCTS_FEATURES = (context: Context): string[][] => [
    [context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_FEATURES1'))],
    [context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_FEATURES2'))],
    [context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_FEATURES3'))],
    [context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_FEATURES4'))],
    [context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_FEATURES5'))]];
  /**
   * Section products comment list.
   */
  static readonly SECTION_PRODUCTS_COMMENTS = (context: Context): string =>
    context.resourceManager.getStringSync($r('app.string.HOME_REVIEW'));
  /**
   * Welfare grid column gutter list.
   */
  static readonly WELFARE_GRID_COL_GUTTERS: number[] = [10, 12, 8, 4];
  /**
   * Selection image aspect ratio.
   */
  static readonly SELECTION_IMG_ASPECT_RATIO: number = 1.29;
  /**
   * Flash sale product image list.
   */
  static readonly FLASH_SALE_PRODUCT_IMAGES: Resource[] = [$r('app.media.flash_sale_product_1'),
    $r('app.media.flash_sale_product_2'), $r('app.media.flash_sale_product_3'), $r('app.media.flash_sale_product_4'),
    $r('app.media.flash_sale_product_4'), $r('app.media.flash_sale_product_4')];
  /**
   * Flash sale product title list.
   */
  static readonly FLASH_SALE_PRODUCT_TITLES = (context: Context): string[][] => [['HUAWEI Mate 50'], ['Mate Xs 2'],
    ['HUAWEI nova \u200B1\u200B0',
      context.resourceManager.getStringSync($r('app.string.Hot_Seller'))],
    ['MateBook E Go', context.resourceManager.getStringSync($r('app.string.New_Launch'))],
    ['MateBook E Go', context.resourceManager.getStringSync($r('app.string.New_Launch'))],
    ['MateBook E Go', context.resourceManager.getStringSync($r('app.string.New_Launch'))]];
  /**
   * Flash sale product info list.
   */
  static readonly FLASH_SALE_PRODUCT_INFO = (context: Context): string[] => [
    context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_INFO1')),
    context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_INFO6')),
    context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_INFO4')),
    context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_INFO5')),
    context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_INFO5')),
    context.resourceManager.getStringSync($r('app.string.SECTION_PRODUCTS_INFO5')),
  ];
  /**
   * Flash sale product feature list.
   */
  static readonly FLASH_SALE_PRODUCT_FEATURES = (context: Context): string[][] => [
    [context.resourceManager.getStringSync($r('app.string.FLASH_SALE_PRODUCT_FEATURES1')),
      context.resourceManager.getStringSync($r('app.string.FLASH_SALE_PRODUCT_FEATURES2'))],
    [context.resourceManager.getStringSync($r('app.string.FLASH_SALE_PRODUCT_FEATURES3'))],
    [context.resourceManager.getStringSync($r('app.string.FLASH_SALE_PRODUCT_FEATURES4'))],
    [context.resourceManager.getStringSync($r('app.string.FLASH_SALE_PRODUCT_FEATURES5'))],
    [context.resourceManager.getStringSync($r('app.string.FLASH_SALE_PRODUCT_FEATURES5'))],
    [context.resourceManager.getStringSync($r('app.string.FLASH_SALE_PRODUCT_FEATURES5'))]];
  /**
   * Flash sale product price list.
   */
  static readonly FLASH_SALE_PRODUCT_PRICES: string[] = ['￥4999', '￥9999', '￥2949', '￥4899', '￥4899', '￥4899'];
  /**
   * Flash sale original price list.
   */
  static readonly FLASH_SALE_PRODUCT_ORIGINAL_PRICES: string[] = ['￥5488', '', '￥2999', '￥4999', '￥4999', '￥4999'];
  /**
   * Flash sale image percent.
   */
  static readonly FLASH_SALE_IMAGE_PERCENT: number[] = [0.41, 0.44];
  /**
   * Flash sale introduction column height.
   */
  static readonly FLASH_SALE_INTRO_COL_HEIGHT: number = 134;
  /**
   * Home header list spaces.
   */
  static readonly HOME_HEADER_LIST_SPACES: number[] = [14, 16, 32];
  /**
   * Nine.
   */
  static readonly NINE: number = 9;
}