/*
 *  Copyright (c) 2025 Huawei Device Co., Ltd.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

import { relationalStore } from '@kit.ArkData';
import { AccountTable } from '../viewmodel/BillViewModel';

/**
 * Common constants for all features.
 */
export default class CommonConstants {
  /**
   * Search font weight.
   */
  static readonly SEARCH_FONT_WEIGHT: number = 10;
  /**
   * The font family of search component.
   */
  static readonly SEARCH_FONT_FAMILY: string = 'serif';
  /**
   * The layout weight of the list title.
   */
  static readonly TITLE_LAYOUT_WEIGHT: number = 1;
  /**
   * The max lines of the list title.
   */
  static readonly TITLE_MAX_LINES: number = 1;
  /**
   * Set component layout weight.
   */
  static readonly WEIGHT: number = 1;
  /**
   *  The contacts detail page url.
   */
  static readonly PAGE_DETAIL_URL: string = 'pages/ContactDetailPage';
  /**
   * The contacts editing page url.
   */
  static readonly ADD_EDIT_URL: string = 'pages/ContactAddAndEditPage';
  /**
   * The contacts list page url.
   */
  static readonly LIST_PAGE_URL: string = 'pages/ContactHomePage';
  /**
   * The contacts delete page url.
   */
  static readonly DELETE_PAGE_URL: string = 'pages/ContactDeletePage';
  /**
   * The key to transfer parameters when replace pages.
   */
  static readonly KEY_PARAM_DATA: string = 'data';
  /**
   * The maximum component width.
   */
  static readonly PERCENTAGE_MAX: string = '100%';
  /**
   * Eighty percent assembly width.
   */
  static readonly PERCENTAGE_HEIGHT_MAX: string = '80%';
  /**
   * Delete page title width.
   */
  static readonly PAGE_TITLE_WIDTH: string = '90%';
  /**
   * Edit the width of the input box on the page.
   */
  static readonly EDIT_INPUT_WIDTH: string = '80%';
  /**
   * The contacts details page components width.
   */
  static readonly DETAIL_ITEM_WIDTH: string = '70%';
  /**
   * Height of the address book list.
   */
  static readonly CONTACTS_LIST_HEIGHT: string = '90%';
  /**
   * The width of the bottom component.
   */
  static readonly BOTTOM_COMPONENT_WIDTH: string = '50%';
  /**
   * The contacts details page top height.
   */
  static readonly DETAIL_TOP_HEIGHT: string = '30%';
  /**
   * The percentage of list component.
   */
  static readonly LIST_WIDTH_PERCENT: string = '93.3%';
  /**
   * The contacts list divider left margin.
   */
  static readonly CONTACTS_LIST_DIVIDER: string = '16%';
  /**
   * The contacts list divider left margin.
   */
  static readonly CONTACTS_EMPTY_TOP: string = '17.8%';
  /**
   * Contact database key.
   */
  static readonly CONTACTS_DATABASE_KEY: string = 'contactsKey';
  /**
   * Ability name.
   */
  static readonly ABILITY_NAME: string = 'EntryAbility';
  /**
   * Detail page id.
   */
  static readonly DETAIL_PAGE_ID: string = 'detailId';
  /**
   * Phone components id.
   */
  static readonly PHONE_COMPONENTS_ID: number = 1;
  /**
   * Email components id.
   */
  static readonly Email_COMPONENTS_ID: number = 2;
  /**
   * Note components id.
   */
  static readonly Note_COMPONENTS_ID: number = 3;
  /**
   * Device list group.
   */
  static readonly DEVICE_GROUP: string = 'deviceGroup';
  /**
   * Even column.
   */
  static readonly EVEN_COLUMN: number = 2;
  /**
   * Indicates whether to edit the page.
   */
  static readonly IS_EDIT: string = 'isEdit';
  /**
   * Key of details page information.
   */
  static readonly DETAIL_INFO_KEY: string = 'key';
  /**
   * Monitors distributed database changes.
   */
  static readonly DATA_CHANGE: string = 'dataChange';
  /**
   * Start ability callback.
   */
  static readonly EXIT: string = 'exit';
  /**
   * Start ability callback id.
   */
  static readonly SUBSCRIBE_ID: number = 100;
  /**
   * The mode of start device discovery info.
   */
  static readonly INFO_MODE: number = 0xAA;
  /**
   * The frequency of start device discovery info.
   */
  static readonly INFO_FREQ: number = 2;
  /**
   * Discover device list.
   */
  static readonly DISCOVER_DEVICE_LIST: string = 'discoverDeviceList';
  /**
   * Zero.
   */
  static readonly ZERO: string = '0';
  /**
   * One.
   */
  static readonly ONE: number = 1;
  /**
   * Trusted device list.
   */
  static readonly TRUSTED_DEVICE_LIST: string = 'trustedDeviceList';
  /**
   * Subscribe ID range.
   */
  static readonly SUBSCRIBE_ID_RANGE: number = 65536;
  /**
   * Auth type.
   */
  static readonly AUTH_TYPE: number = 1;
  /**
   * Subscribe mode.
   */
  static readonly SUBSCRIBE_MODE: number = 0xAA;
  /**
   * Subscribe medium.
   */
  static readonly SUBSCRIBE_MEDIUM: number = 0;
  /**
   * Subscribe freq.
   */
  static readonly SUBSCRIBE_FREQ: number = 2;
  /**
   * Subscribe capability.
   */
  static readonly SUBSCRIBE_CAPABILITY: number = 0;
  /**
   * Font weight.
   */
  static readonly FONT_WEIGHT_500: number = 500;
  /**
   * Invalid Index.
   */
  static readonly INVALID_INDEX: number = -1;
  /**
   * One hundred percent.
   */
  static readonly FULL_PERCENT: string = '100%';
  /**
   * Number one.
   */
  static readonly NUMBER_ONE: number = 1;
  /**
   * Device name width.
   */
  static readonly DEVICE_NAME_WIDTH: string = '78%';
  /**
   * Select icon width.
   */
  static readonly SELECT_ICON_WIDTH: string = '8%';
  /**
   * Localhost name.
   */
  static readonly LOCALHOST_NAME: string = 'Local Host';
  /**
   * Entry ability.
   */
  static readonly ENTRY_ABILITY: string = 'EntryAbility';
  /**
   * Ability name.
   */
  static readonly BUNDLE_NAME: string = 'com.example.crossDeviceDataSynchronization';
  /**
   * App name.
   */
  static readonly APP_NAME: string = 'Distributed Contacts';
  /**
   * Contacts name max length.
   */
  static readonly CONTACTS_NAME_MAX_LENGTH: number = 10;
  /**
   * Contacts tel max length.
   */
  static readonly CONTACTS_TEL_MAX_LENGTH: number = 11;
  /**
   * Contacts detail input box max length.
   */
  static readonly CONTACTS_DETAIL_MAX: number = 20;
  /**
   * Distributed mail image storage location.
   */
  static readonly MAIL_DISTRIBUTED_PATH: string = '/data/storage/el2/distributedfiles/';
  /**
   * File name for storing cross-end photos.
   */
  static readonly PICTURE_NAME: string = 'pictureName';
  /**
   * Supports transfer.
   */
  static readonly CAN_CONTINUATION: string = 'true';
  /**
   * Forwarding is not supported.
   */
  static readonly NO_CONTINUATION: string = 'false';
  /**
   * With picture streaming.
   */
  static readonly CAN_PHOTO: string = 'true';
  /**
   * Transfer without picture.
   */
  static readonly NO_PHOTO: string = 'false';
  /**
   * Image stream size.
   */
  static readonly IMAGE_STREAM_SIZE: number = 10240000;
  /**
   * Bottom toolbar picture spacing.
   */
  static readonly BOTTOM_IMAGE_SPACE: number = 24;
  static readonly FILE_BUFFER_SIZE: number = 4096;
  static readonly APPENDIX_LIST_SPACE: number = 5;
  static readonly APPENDIX_LIST_START_MARGIN: number = 15;
  static readonly APPENDIX_LIST_END_MARGIN: number = 15;
  /**
   * Rdb database config.
   */
  static readonly STORE_CONFIG: relationalStore.StoreConfig = {
    name: 'database.db',
    securityLevel: relationalStore.SecurityLevel.S1,
  };
  /**
   * Account table config.
   */
  static readonly ACCOUNT_TABLE: AccountTable = {
    tableName: 'accountTable',
    sqlCreate: 'CREATE TABLE IF NOT EXISTS accountTable(id TEXT, accountType INTEGER, typeText TEXT, amount INTEGER)',
    columns: ['id', 'accountType', 'typeText', 'amount'],
  };
  /**
   * Search text of Search component.
   */
  static readonly SEARCH_TEXT = $r('app.string.search');
  /**
   * toast text of prompt component.
   */
  static readonly TOAST_TEXT_1 = $r('app.string.TOAST_TEXT_1');
  static readonly TOAST_TEXT_2 = $r('app.string.TOAST_TEXT_2');
  /**
   * Component size.
   */
  static readonly DIALOG_HEIGHT = '55%';
  static readonly TABS_HEIGHT = '45%';
  static readonly MINIMUM_SIZE = 0;
  static readonly FULL_SIZE = 1;
  static readonly PROMPT_BOTTOM = '70vp';
  /**
   * Component location.
   */
  static readonly EDIT_POSITION_X = '80%';
  static readonly EDIT_POSITION_Y = '90%';
  static readonly DELETE_POSITION_X = '50%';
  static readonly DELETE_POSITION_Y = '90%';
  /**
   * Log tag.
   */
  static readonly RDB_TAG = '[Debug.Rdb]';
  static readonly TABLE_TAG = '[Debug.AccountTable]';
  static readonly INDEX_TAG = '[Debug.Index]';
}