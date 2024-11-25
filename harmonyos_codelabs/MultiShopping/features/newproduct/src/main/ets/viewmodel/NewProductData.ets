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

class AspectRatio {
  sm?: number;
  md?: number;
  lg?: number;
}

const aspectRatio: AspectRatio = {
  sm: 0.84,
  md: 0.81,
  lg: 1.31
}

const swiperImage: Resource[] = [
  $r('app.media.ic_banner'),
  $r('app.media.ic_banner01'),
  $r('app.media.ic_banner02'),
  $r('app.media.ic_banner03'),
  $r('app.media.ic_banner04')
]

const classifyTitle: Resource[] = [
  $r('app.string.swiper_product_boutique'),
  $r('app.string.swiper_product_phone'),
  $r('app.string.swiper_product_clothes'),
  $r('app.string.swiper_product_match'),
  $r('app.string.swiper_product_furnishing')
]

class ProductDataModel {
  img: string = '';
  comment?: Resource;
  des?: Resource;
  user?: Resource;
}

const productData: ProductDataModel[] = [
  {
    img: 'common/ic_product01.png',
    comment: $r('app.string.product1_comment'),
    des: $r('app.string.product1_des'),
    user: $r('app.string.product1_user')
  },
  {
    img: 'common/ic_product02.png',
    comment: $r('app.string.product2_comment'),
    des: $r('app.string.product2_des'),
    user: $r('app.string.product2_user')
  },
  {
    img: 'common/ic_product03.png',
    comment: $r('app.string.product3_comment'),
    des: $r('app.string.product3_des'),
    user: $r('app.string.product3_user')
  },
  {
    img: 'common/ic_product04.png',
    comment: $r('app.string.product4_comment'),
    des: $r('app.string.product4_des'),
    user: $r('app.string.product4_user')
  },
  {
    img: 'common/ic_product02.png',
    comment: $r('app.string.product5_comment'),
    des: $r('app.string.product5_des'),
    user: $r('app.string.product5_user')
  },
  {
    img: 'common/ic_product04.png',
    comment: $r('app.string.product6_comment'),
    des: $r('app.string.product6_des'),
    user: $r('app.string.product6_user')
  }
]

export { aspectRatio, swiperImage, classifyTitle, productData, ProductDataModel };