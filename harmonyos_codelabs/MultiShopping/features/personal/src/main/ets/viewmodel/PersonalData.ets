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

import { OrderType } from '@ohos/common';
import { IconButtonModel } from './IconButtonModel';

const orderButton: IconButtonModel[] = [
  new IconButtonModel(
    $r('app.media.ic_credit_card'),
    $r('app.string.pending_payment'),
    OrderType.PAYMENT,
    0
  ),
  new IconButtonModel(
    $r('app.media.ic_ship'),
    $r('app.string.pending_shipment'),
    OrderType.SHIP,
    0
  ),
  new IconButtonModel(
    $r('app.media.ic_flat'),
    $r('app.string.pending_receive'),
    OrderType.RECEIPT,
    0
  ),
  new IconButtonModel(
    $r('app.media.ic_evaluation'),
    $r('app.string.pending_evaluated'),
    OrderType.EVALUATION,
    0
  ),
  new IconButtonModel(
    $r('app.media.ic_sale'),
    $r('app.string.pending_after_sales'),
    OrderType.SALE,
    0
  )
]

const memberButton: IconButtonModel[] = [
  new IconButtonModel($r('app.media.ic_member'), $r('app.string.member_type')),
  new IconButtonModel($r('app.media.ic_score'), $r('app.string.bonus_store')),
  new IconButtonModel($r('app.media.ic_interact'), $r('app.string.interactive')),
  new IconButtonModel($r('app.media.ic_card'), $r('app.string.card_coupon'))
]

const activityButton: IconButtonModel[] = [
  new IconButtonModel($r('app.media.ic_official'), $r('app.string.official_service')),
  new IconButtonModel($r('app.media.ic_cash'), $r('app.string.cash')),
  new IconButtonModel($r('app.media.ic_trade'), $r('app.string.trade_in')),
  new IconButtonModel($r('app.media.ic_my_evaluation'), $r('app.string.my_evaluation'))
]

class LiveDataModel {
  liveId: string = '';
  living: boolean = false;
  watchDesc?: Resource;
  previewIcon: string = '';
  liverIcon: string = '';
  liverName?: Resource;
  liverDesc?: Resource;
}

const liveData: LiveDataModel[] = [
  {
    liveId: '1',
    living: true,
    watchDesc: $r('app.string.live_data1_watch_desc'),
    previewIcon: 'common/food.png',
    liverIcon: 'common/ic_person.png',
    liverName: $r('app.string.live_data1_liver_name'),
    liverDesc: $r('app.string.live_data1_liver_desc')
  },
  {
    liveId: '2',
    living: true,
    watchDesc: $r('app.string.live_data2_watch_desc'),
    previewIcon: 'common/scenery.png',
    liverIcon: 'common/ic_person.png',
    liverName: $r('app.string.live_data2_liver_name'),
    liverDesc: $r('app.string.live_data2_liver_desc')
  },
  {
    liveId: '3',
    living: false,
    watchDesc: $r('app.string.live_data3_watch_desc'),
    previewIcon: 'common/food.png',
    liverIcon: 'common/ic_person.png',
    liverName: $r('app.string.live_data3_liver_name'),
    liverDesc: $r('app.string.live_data3_liver_desc')
  },
  {
    liveId: '4',
    living: true,
    watchDesc: $r('app.string.live_data4_watch_desc'),
    previewIcon: 'common/scenery.png',
    liverIcon: 'common/ic_person.png',
    liverName: $r('app.string.live_data4_liver_name'),
    liverDesc: $r('app.string.live_data4_liver_desc')
  },
  {
    liveId: '5',
    living: true,
    watchDesc: $r('app.string.live_data5_watch_desc'),
    previewIcon: 'common/food.png',
    liverIcon: 'common/ic_person.png',
    liverName: $r('app.string.live_data5_liver_name'),
    liverDesc: $r('app.string.live_data5_liver_desc')
  },
  {
    liveId: '6',
    living: true,
    watchDesc: $r('app.string.live_data6_watch_desc'),
    previewIcon: 'common/scenery.png',
    liverIcon: 'common/ic_person.png',
    liverName: $r('app.string.live_data6_liver_name'),
    liverDesc: $r('app.string.live_data6_liver_desc')
  },
  {
    liveId: '7',
    living: true,
    watchDesc: $r('app.string.live_data7_watch_desc'),
    previewIcon: 'common/food.png',
    liverIcon: 'common/ic_person.png',
    liverName: $r('app.string.live_data7_liver_name'),
    liverDesc: $r('app.string.live_data7_liver_desc')
  },
  {
    liveId: '8',
    living: true,
    watchDesc: $r('app.string.live_data8_watch_desc'),
    previewIcon: 'common/scenery.png',
    liverIcon: 'common/ic_person.png',
    liverName: $r('app.string.live_data8_liver_name'),
    liverDesc: $r('app.string.live_data8_liver_desc')
  }
]

export { orderButton, memberButton, activityButton, liveData, LiveDataModel };