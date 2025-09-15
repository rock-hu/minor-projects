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

export class AddressInfoParams {
  id?: string;
  updateTime?: string;
  isDefault?: number;
  province?: Resource;
  createTime?: string;
  phone?: string;
  city?: Resource;
  postCode?: string;
  name?: Resource;
  detailAddress?: Resource;
  region?: Resource;
  addressId?: string;
  post_code?: string;
}

const addressInfo: AddressInfoParams[] = [
  {
    id: '1',
    updateTime: '2022-10-22T10:10:10.222Z',
    isDefault: 1,
    province: $r('app.string.address1_province'),
    createTime: '2022-10-22T10:10:10.222Z',
    phone: '131****2168',
    city: $r('app.string.address1_city'),
    postCode: '246100',
    name: $r('app.string.address1_name'),
    detailAddress: $r('app.string.address1_detail_address'),
    region: $r('app.string.address1_region'),
    addressId: '1'
  },
  {
    id: '1',
    updateTime: '2022-10-22T10:10:10.222Z',
    isDefault: 0,
    province: $r('app.string.address2_province'),
    createTime: '2022-10-22T10:10:10.222Z',
    phone: '182****0193',
    city: $r('app.string.address2_city'),
    post_code: '241200',
    name: $r('app.string.address2_name'),
    detailAddress: $r('app.string.address2_detail_address'),
    region: $r('app.string.address2_region'),
    addressId: '3'
  }
];

const payEnum = [$r('app.string.accounts_payable'),$r('app.string.order_money'),$r('app.string.order_money')];

const statusEnum = [$r('app.string.wait_pay'),$r('app.string.seller_shipped'),$r('app.string.completed')];

export { addressInfo, payEnum, statusEnum }