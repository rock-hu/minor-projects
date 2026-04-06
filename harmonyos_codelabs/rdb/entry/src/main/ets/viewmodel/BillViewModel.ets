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

export class AccountData {
  id: string = '';
  accountType: number = 0;
  typeText: string = '';
  amount: number = 0;
}

export interface AccountItem {
  icon: Resource;
  accountType: number;
  typeText: string;
}

export const PayList: Array<AccountItem> = [
  {
    icon: $r('sys.symbol.fork_knife_fill'),
    accountType: 0,
    typeText: 'foods'
  },
  {
    icon: $r('sys.symbol.cart_fill'),
    accountType: 0,
    typeText: 'snacks'
  },
  {
    icon: $r("sys.symbol.fuelpump_fill"),
    accountType: 0,
    typeText: 'fuel'
  },
  {
    icon: $r('sys.symbol.tropicaltrees_and_sand'),
    accountType: 0,
    typeText: 'travel'
  },
  {
    icon: $r('sys.symbol.gamecontroller_fill'),
    accountType: 0,
    typeText: 'games'
  },
  {
    icon: $r('sys.symbol.pawprint_fill'),
    accountType: 0,
    typeText: 'pets'
  }
];

export const EarnList: Array<AccountItem> = [
  {
    icon: $r('sys.symbol.case_fill'),
    accountType: 1,
    typeText: 'income'
  },
  {
    icon: $r('sys.symbol.creditcard_fill'),
    accountType: 1,
    typeText: 'invest'
  }
];

export const ImageList: Record<string, Resource> = {
  'foods': $r('sys.symbol.fork_knife_fill'),
  'snacks': $r('sys.symbol.cart_fill'),
  'fuel': $r("sys.symbol.fuelpump_fill"),
  'travel': $r('sys.symbol.tropicaltrees_and_sand'),
  'games': $r('sys.symbol.gamecontroller_fill'),
  'pets': $r('sys.symbol.pawprint_fill'),
  'income': $r('sys.symbol.case_fill'),
  'invest': $r('sys.symbol.creditcard_fill')
};

export const TextList: Record<string, Resource> = {
  'foods': $r('app.string.foods'),
  'snacks': $r('app.string.snacks'),
  'fuel': $r('app.string.fuel'),
  'travel': $r('app.string.travel'),
  'games': $r('app.string.games'),
  'pets': $r('app.string.pets'),
  'income': $r('app.string.income'),
  'invest': $r('app.string.invest')
};

export interface AccountTable {
  tableName: string;
  sqlCreate: string;
  columns: string[];
}