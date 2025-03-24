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

import { Commodity } from './CommodityModel';
import { Order } from './OrderModel';
import { Product } from './ProductModel';

/**
 * Application simulation data can be obtained from the server by developers.
 */
const commodityData: Commodity[] = [
  {
    detail: ["common/product001.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity1_color_key1'), value: $r('app.string.commodity1_color_value1'), image: '' },
          { key: $r('app.string.commodity1_color_key2'), value: $r('app.string.commodity1_color_value2'), image: '' },
          { key: $r('app.string.commodity1_color_key3'), value: $r('app.string.commodity1_color_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity1_version_key1'), value: $r('app.string.commodity1_version_value1'), image: '' },
          { key: $r('app.string.commodity1_version_key2'), value: $r('app.string.commodity1_version_value2'), image: '' },
          { key: $r('app.string.commodity1_version_key3'), value: $r('app.string.commodity1_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity1_network_key1'), value: $r('app.string.commodity1_network_value1'), image: '' },
          { key: $r('app.string.commodity1_network_key2'), value: $r('app.string.commodity1_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity1_type_key1'), value: $r('app.string.commodity1_type_value1'), image: '' },
          { key: $r('app.string.commodity1_type_key2'), value: $r('app.string.commodity1_type_value2'), image: '' }
        ]
      }
    ],
    category_id: "1",
    price: 6399,
    title: $r('app.string.commodity1_title'),
    images: ["common/product001.png"],
    id: "1",
    description: $r('app.string.commodity1_description'),
    promotion: $r('app.string.commodity1_promotion')
  },
  {
    detail: ["common/product002.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity2_color_key1'), value: $r('app.string.commodity2_color_value1'), image: '' },
          { key: $r('app.string.commodity2_color_key2'), value: $r('app.string.commodity2_color_value2'), image: '' },
          { key: $r('app.string.commodity2_color_key3'), value: $r('app.string.commodity2_color_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity2_version_key1'), value: $r('app.string.commodity2_version_value1'), image: '' },
          { key: $r('app.string.commodity2_version_key2'), value: $r('app.string.commodity2_version_value2'), image: '' },
          { key: $r('app.string.commodity2_version_key3'), value: $r('app.string.commodity2_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity2_network_key1'), value: $r('app.string.commodity2_network_value1'), image: '' },
          { key: $r('app.string.commodity2_network_key2'), value: $r('app.string.commodity2_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity2_type_key1'), value: $r('app.string.commodity2_type_value1'), image: '' },
          { key: $r('app.string.commodity2_type_key2'), value: $r('app.string.commodity2_type_value2'), image: '' }
        ]
      }
    ],
    category_id: "1",
    price: 5199,
    title: $r('app.string.commodity2_title'),
    images: ["common/product002.png"],
    id: "2",
    description: $r('app.string.commodity2_description'),
    promotion: $r('app.string.commodity2_promotion')
  },
  {
    detail: ["common/product003.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity3_color_key1'), value: $r('app.string.commodity3_color_value1'), image: '' },
          { key: $r('app.string.commodity3_color_key2'), value: $r('app.string.commodity3_color_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity3_version_key1'), value: $r('app.string.commodity3_version_value1'), image: '' },
          { key: $r('app.string.commodity3_version_key2'), value: $r('app.string.commodity3_version_value2'), image: '' },
          { key: $r('app.string.commodity3_version_key3'), value: $r('app.string.commodity3_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity3_network_key1'), value: $r('app.string.commodity3_network_value1'), image: '' },
          { key: $r('app.string.commodity3_network_key2'), value: $r('app.string.commodity3_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity3_type_key1'), value: $r('app.string.commodity3_type_value1'), image: '' },
          { key: $r('app.string.commodity3_type_key2'), value: $r('app.string.commodity3_type_value2'), image: '' }
        ]
      }
    ],
    category_id: "3",
    price: 6588,
    title: $r('app.string.commodity3_title'),
    images: ["common/product003.png"],
    id: "3",
    description: $r('app.string.commodity3_description'),
    promotion: $r('app.string.commodity3_promotion')
  },
  {
    detail: ["common/product004.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity4_color_key1'), value: $r('app.string.commodity4_color_value1'), image: '' },
          { key: $r('app.string.commodity4_color_key2'), value: $r('app.string.commodity4_color_value2'), image: '' },
          { key: $r('app.string.commodity4_color_key3'), value: $r('app.string.commodity4_color_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity4_version_key1'), value: $r('app.string.commodity4_version_value1'), image: '' },
          { key: $r('app.string.commodity4_version_key2'), value: $r('app.string.commodity4_version_value2'), image: '' },
          { key: $r('app.string.commodity4_version_key3'), value: $r('app.string.commodity4_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity4_network_key1'), value: $r('app.string.commodity4_network_value1'), image: '' },
          { key: $r('app.string.commodity4_network_key2'), value: $r('app.string.commodity4_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity4_type_key1'), value: $r('app.string.commodity4_type_value1'), image: '' },
          { key: $r('app.string.commodity4_type_key2'), value: $r('app.string.commodity4_type_value2'), image: '' }
        ]
      }
    ],
    category_id: '',
    price: 3999,
    title: $r('app.string.commodity4_title'),
    images: ["common/product004.png"],
    id: "4",
    description: $r('app.string.commodity4_description'),
    promotion: $r('app.string.commodity4_promotion')
  },
  {
    detail: ["common/product005.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity5_color_key1'), value: $r('app.string.commodity5_color_value1'), image: '' },
          { key: $r('app.string.commodity5_color_key2'), value: $r('app.string.commodity5_color_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity5_version_key1'), value: $r('app.string.commodity5_version_value1'), image: '' },
          { key: $r('app.string.commodity5_version_key2'), value: $r('app.string.commodity5_version_value2'), image: '' },
          { key: $r('app.string.commodity5_version_key3'), value: $r('app.string.commodity5_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity5_network_key1'), value: $r('app.string.commodity5_network_value1'), image: '' },
          { key: $r('app.string.commodity5_network_key2'), value: $r('app.string.commodity5_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity5_type_key1'), value: $r('app.string.commodity5_type_value1'), image: '' },
          { key: $r('app.string.commodity5_type_key2'), value: $r('app.string.commodity5_type_value2'), image: '' }
        ]
      }
    ],
    category_id: '',
    price: 1899,
    title: $r('app.string.commodity5_title'),
    images: ["common/product005.png"],
    id: "5",
    description: $r('app.string.commodity5_description'),
    promotion: $r('app.string.commodity5_promotion')
  },
  {
    detail: ["common/product006.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity6_color_key1'), value: $r('app.string.commodity6_color_value1'), image: '' },
          { key: $r('app.string.commodity6_color_key2'), value: $r('app.string.commodity6_color_value2'), image: '' },
          { key: $r('app.string.commodity6_color_key3'), value: $r('app.string.commodity6_color_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity6_version_key1'), value: $r('app.string.commodity6_version_value1'), image: '' },
          { key: $r('app.string.commodity6_version_key2'), value: $r('app.string.commodity6_version_value2'), image: '' },
          { key: $r('app.string.commodity6_version_key3'), value: $r('app.string.commodity6_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity6_network_key1'), value: $r('app.string.commodity6_network_value1'), image: '' },
          { key: $r('app.string.commodity6_network_key2'), value: $r('app.string.commodity6_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity6_type_key1'), value: $r('app.string.commodity6_type_value1'), image: '' },
          { key: $r('app.string.commodity6_type_key2'), value: $r('app.string.commodity6_type_value2'), image: '' }
        ]
      }
    ],
    category_id: '',
    price: 2199,
    title: $r('app.string.commodity6_title'),
    images: ["common/product006.png"],
    id: "6",
    description: $r('app.string.commodity6_description'),
    promotion: $r('app.string.commodity6_promotion')
  },
  {
    detail: ["common/product006.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity7_color_key1'), value: $r('app.string.commodity7_color_value1'), image: '' },
          { key: $r('app.string.commodity7_color_key2'), value: $r('app.string.commodity7_color_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity7_version_key1'), value: $r('app.string.commodity7_version_value1'), image: '' },
          { key: $r('app.string.commodity7_version_key2'), value: $r('app.string.commodity7_version_value2'), image: '' },
          { key: $r('app.string.commodity7_version_key3'), value: $r('app.string.commodity7_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity7_network_key1'), value: $r('app.string.commodity7_network_value1'), image: '' },
          { key: $r('app.string.commodity7_network_key2'), value: $r('app.string.commodity7_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity7_type_key1'), value: $r('app.string.commodity7_type_value1'), image: '' },
          { key: $r('app.string.commodity7_type_key2'), value: $r('app.string.commodity7_type_value2'), image: '' }
        ]
      }
    ],
    category_id: "1",
    price: 3299,
    title: $r('app.string.commodity7_title'),
    images: ["common/product006.png"],
    id: "7",
    description: $r('app.string.commodity7_description'),
    promotion: $r('app.string.commodity7_promotion')
  },
  {
    detail: ["common/product006.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity8_color_key1'), value: $r('app.string.commodity8_color_value1'), image: '' },
          { key: $r('app.string.commodity8_color_key2'), value: $r('app.string.commodity8_color_value2'), image: '' },
          { key: $r('app.string.commodity8_color_key3'), value: $r('app.string.commodity8_color_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity8_version_key1'), value: $r('app.string.commodity8_version_value1'), image: '' },
          { key: $r('app.string.commodity8_version_key2'), value: $r('app.string.commodity8_version_value2'), image: '' },
          { key: $r('app.string.commodity8_version_key3'), value: $r('app.string.commodity8_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity8_network_key1'), value: $r('app.string.commodity8_network_value1'), image: '' },
          { key: $r('app.string.commodity8_network_key2'), value: $r('app.string.commodity8_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity8_type_key1'), value: $r('app.string.commodity8_type_value1'), image: '' },
          { key: $r('app.string.commodity8_type_key2'), value: $r('app.string.commodity8_type_value2'), image: '' }
        ]
      }
    ],
    category_id: "1",
    price: 12999,
    title: $r('app.string.commodity8_title'),
    images: ["common/product006.png"],
    id: "8",
    description: $r('app.string.commodity8_description'),
    promotion: $r('app.string.commodity8_promotion')
  },
  {
    detail: ["common/product003.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity9_color_key1'), value: $r('app.string.commodity9_color_value1'), image: '' },
          { key: $r('app.string.commodity9_color_key2'), value: $r('app.string.commodity9_color_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity9_version_key1'), value: $r('app.string.commodity9_version_value1'), image: '' },
          { key: $r('app.string.commodity9_version_key2'), value: $r('app.string.commodity9_version_value2'), image: '' },
          { key: $r('app.string.commodity9_version_key3'), value: $r('app.string.commodity9_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity9_network_key1'), value: $r('app.string.commodity9_network_value1'), image: '' },
          { key: $r('app.string.commodity9_network_key2'), value: $r('app.string.commodity9_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity9_type_key1'), value: $r('app.string.commodity9_type_value1'), image: '' },
          { key: $r('app.string.commodity9_type_key2'), value: $r('app.string.commodity9_type_value2'), image: '' }
        ]
      }
    ],
    category_id: "3",
    price: 6588,
    title: $r('app.string.commodity9_title'),
    images: ["common/product003.png"],
    id: "9",
    description: $r('app.string.commodity9_description'),
    promotion: $r('app.string.commodity9_promotion')
  },
  {
    detail: ["common/product004.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity10_color_key1'), value: $r('app.string.commodity10_color_value1'), image: '' },
          { key: $r('app.string.commodity10_color_key2'), value: $r('app.string.commodity10_color_value2'), image: '' },
          { key: $r('app.string.commodity10_color_key3'), value: $r('app.string.commodity10_color_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity10_version_key1'), value: $r('app.string.commodity10_version_value1'), image: '' },
          { key: $r('app.string.commodity10_version_key2'), value: $r('app.string.commodity10_version_value2'), image: '' },
          { key: $r('app.string.commodity10_version_key3'), value: $r('app.string.commodity10_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity10_network_key1'), value: $r('app.string.commodity10_network_value1'), image: '' },
          { key: $r('app.string.commodity10_network_key2'), value: $r('app.string.commodity10_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity10_type_key1'), value: $r('app.string.commodity10_type_value1'), image: '' },
          { key: $r('app.string.commodity10_type_key2'), value: $r('app.string.commodity10_type_value2'), image: '' }
        ]
      }
    ],
    category_id: '',
    price: 3999,
    title: $r('app.string.commodity10_title'),
    images: ["common/product004.png"],
    id: "10",
    description: $r('app.string.commodity10_description'),
    promotion: $r('app.string.commodity10_promotion')
  },
  {
    detail: ["common/product003.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity11_color_key1'), value: $r('app.string.commodity11_color_value1'), image: '' },
          { key: $r('app.string.commodity11_color_key2'), value: $r('app.string.commodity11_color_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity11_version_key1'), value: $r('app.string.commodity11_version_value1'), image: '' },
          { key: $r('app.string.commodity11_version_key2'), value: $r('app.string.commodity11_version_value2'), image: '' },
          { key: $r('app.string.commodity11_version_key3'), value: $r('app.string.commodity11_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity11_network_key1'), value: $r('app.string.commodity11_network_value1'), image: '' },
          { key: $r('app.string.commodity11_network_key2'), value: $r('app.string.commodity11_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity11_type_key1'), value: $r('app.string.commodity11_type_value1'), image: '' },
          { key: $r('app.string.commodity11_type_key2'), value: $r('app.string.commodity11_type_value2'), image: '' }
        ]
      }
    ],
    category_id: "3",
    price: 6588,
    title: $r('app.string.commodity11_title'),
    images: ["common/product003.png"],
    id: "11",
    description: $r('app.string.commodity11_description'),
    promotion: $r('app.string.commodity11_promotion')
  },
  {
    detail: ["common/product006.png"],
    specifications: [
      {
        id: $r('app.string.commodity_color_id'),
        title: $r('app.string.commodity_color_title'),
        data: [
          { key: $r('app.string.commodity12_color_key1'), value: $r('app.string.commodity12_color_value1'), image: '' },
          { key: $r('app.string.commodity12_color_key2'), value: $r('app.string.commodity12_color_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_version_id'),
        title: $r('app.string.commodity_version_title'),
        data: [
          { key: $r('app.string.commodity12_version_key1'), value: $r('app.string.commodity12_version_value1'), image: '' },
          { key: $r('app.string.commodity12_version_key2'), value: $r('app.string.commodity12_version_value2'), image: '' },
          { key: $r('app.string.commodity12_version_key3'), value: $r('app.string.commodity12_version_value3'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_network_id'),
        title: $r('app.string.commodity_network_title'),
        data: [
          { key: $r('app.string.commodity12_network_key1'), value: $r('app.string.commodity12_network_value1'), image: '' },
          { key: $r('app.string.commodity12_network_key2'), value: $r('app.string.commodity12_network_value2'), image: '' }
        ]
      },
      {
        id: $r('app.string.commodity_type_id'),
        title: $r('app.string.commodity_type_title'),
        data: [
          { key: $r('app.string.commodity12_type_key1'), value: $r('app.string.commodity12_type_value1'), image: '' },
          { key: $r('app.string.commodity12_type_key2'), value: $r('app.string.commodity12_type_value2'), image: '' }
        ]
      }
    ],
    category_id: "1",
    price: 3299,
    title: $r('app.string.commodity12_title'),
    images: ["common/product006.png"],
    id: "12",
    description: $r('app.string.commodity12_description'),
    promotion: $r('app.string.commodity12_promotion')
  }
]

const shopCartData: Product[] = [
  {
    uid: "1",
    update_time: "2022-12-21T03:34:01.962Z",
    count: 1,
    create_time: "2022-11-02T02:42:31.338Z",
    price: 6399,
    name: $r('app.string.shopCart1_name'),
    id: "6063101776955",
    commodityId: "1",
    img: ["common/product001.png"],
    selected: false,
    specifications: [
      { name: $r('app.string.shopCart1_color_key'), value: $r('app.string.shopCart1_color_value') },
      { name: $r('app.string.shopCart1_version_key'), value: $r('app.string.shopCart1_version_value') }
    ],
    description: $r('app.string.shopCart1_description')
  },
  {
    uid: "1",
    update_time: "2022-12-21T03:34:02.042Z",
    count: 1,
    create_time: "2022-11-03T03:14:33.407Z",
    price: 6588,
    name: $r('app.string.shopCart2_name'),
    id: "7726602128445",
    commodityId: "3",
    img: ["common/product003.png"],
    selected: false,
    specifications: [
      { name: $r('app.string.shopCart2_color_key'), value: $r('app.string.shopCart2_color_value') },
      { name: $r('app.string.shopCart2_version_key'), value: $r('app.string.shopCart2_version_value') }
    ],
    description: $r('app.string.shopCart2_description')
  }
]

const orderData: Order[] = [
  {
    amount: 2099,
    createTime: "2022-11-01T09:39:34.442Z",
    orderTime: "2022-11-01T09:39:34.442Z",
    payTime: "2022-11-01T09:40:07.453Z",
    uid: "1",
    updateTime: "2022-11-01T09:40:07.453Z",
    price: 6588,
    count: 2,
    title: $r('app.string.order1_title'),
    commodityId: 3,
    image: "common/product003.png",
    orderId: "2675124576927",
    status: 2,
    specifications: [
      { name: $r('app.string.order1_color_key'), value: $r('app.string.order1_color_value') },
      { name: $r('app.string.order1_version_key'), value: $r('app.string.order1_version_value') }
    ],
    description: $r('app.string.order1_description')
  },
  {
    amount: 6399,
    createTime: "2022-11-02T11:33:03.414Z",
    orderTime: "2022-11-02T11:33:03.414Z",
    payTime: "2022-11-10T02:15:17.360Z",
    uid: "1",
    updateTime: "2022-11-10T02:15:17.360Z",
    price: 6399,
    count: 1,
    title: $r('app.string.order2_title'),
    commodityId: 1,
    image: "common/product001.png",
    orderId: "8189683677531",
    status: 2,
    specifications: [
      { name: $r('app.string.order2_color_key'), value: $r('app.string.order2_color_value') },
      { name: $r('app.string.order2_version_key'), value: $r('app.string.order2_version_value') }
    ],
    description: $r('app.string.order2_description')
  },
  {
    amount: 6399,
    createTime: "2022-11-01T11:23:51.857Z",
    orderTime: "2022-11-01T11:23:51.857Z",
    payTime: "2022-11-01T11:40:48.365Z",
    uid: "1",
    updateTime: "2022-11-01T11:40:48.365Z",
    price: 6399,
    count: 1,
    title: $r('app.string.order3_title'),
    commodityId: 1,
    image: "common/product001.png",
    orderId: "8479164261530",
    status: 1,
    specifications: [
      { name: $r('app.string.order3_color_key'), value: $r('app.string.order3_color_value') },
      { name: $r('app.string.order3_version_key'), value: $r('app.string.order3_version_value') }
    ],
    description: $r('app.string.order3_description')
  },
  {
    amount: 6399,
    createTime: "2022-10-31T08:24:41.675Z",
    orderTime: "2022-10-31T08:24:41.675Z",
    payTime: "2022-11-02T12:08:50.869Z",
    uid: "1",
    updateTime: "2022-11-02T12:08:50.869Z",
    price: 6399,
    count: 1,
    title: $r('app.string.order4_title'),
    commodityId: 1,
    image: "common/product001.png",
    orderId: "7068271460267",
    status: 2,
    specifications: [
      { name: $r('app.string.order4_color_key'), value: $r('app.string.order4_color_value') },
      { name: $r('app.string.order4_version_key'), value: $r('app.string.order4_version_value') }
    ],
    description: $r('app.string.order4_description')
  },
  {
    amount: 9999,
    createTime: "2022-11-03T08:12:16.761Z",
    orderTime: "2022-11-03T08:12:16.761Z",
    payTime: "2022-11-03T08:12:50.130Z",
    uid: "1",
    updateTime: "2022-11-03T08:12:50.130Z",
    price: 5199,
    count: 1,
    title: $r('app.string.order5_title'),
    commodityId: 2,
    image: "common/product002.png",
    orderId: "3716463840116",
    status: 2,
    specifications: [
      { name: $r('app.string.order5_color_key'), value: $r('app.string.order5_color_value') },
      { name: $r('app.string.order5_version_key'), value: $r('app.string.order5_version_value') }
    ],
    description: $r('app.string.order5_description')
  },
  {
    amount: 6399,
    createTime: "2022-10-31T12:28:33.027Z",
    orderTime: "2022-10-31T12:28:33.027Z",
    payTime: "2022-11-01T09:54:31.241Z",
    uid: "1",
    updateTime: "2022-11-01T09:54:31.241Z",
    price: 6399,
    count: 1,
    title: $r('app.string.order6_title'),
    commodityId: 1,
    image: "common/product001.png",
    orderId: "1691138712761",
    status: 1,
    specifications: [
      { name: $r('app.string.order6_color_key'), value: $r('app.string.order6_color_value') },
      { name: $r('app.string.order6_version_key'), value: $r('app.string.order6_version_value') }
    ],
    description: $r('app.string.order6_description')
  },
  {
    amount: 6399,
    createTime: "2022-12-21T03:34:09.556Z",
    orderTime: "2022-12-21T03:34:09.556Z",
    payTime: "2022-12-21T03:35:03.096Z",
    uid: "1",
    updateTime: "2022-12-21T03:35:03.096Z",
    price: 6399,
    count: 1,
    title: $r('app.string.order7_title'),
    commodityId: 1,
    image: "common/product001.png",
    orderId: "3196746164785",
    status: 1,
    specifications: [
      { name: $r('app.string.order7_color_key'), value: $r('app.string.order7_color_value') },
      { name: $r('app.string.order7_version_key'), value: $r('app.string.order7_version_value') }
    ],
    description: $r('app.string.order7_description')
  },
  {
    amount: 5199,
    createTime: "2022-10-08T03:35:37.331Z",
    orderTime: "2022-10-08T03:35:37.331Z",
    payTime: "2022-10-08T03:38:43.391Z",
    uid: "1",
    updateTime: "2022-10-08T03:38:43.391Z",
    price: 5199,
    count: 1,
    title: $r('app.string.order8_title'),
    commodityId: 2,
    image: "common/product002.png",
    orderId: "9863350027162",
    status: 2,
    specifications: [
      { name: $r('app.string.order8_color_key'), value: $r('app.string.order8_color_value') },
      { name: $r('app.string.order8_version_key'), value: $r('app.string.order8_version_value') }
    ],
    description: $r('app.string.order8_description')
  }
]

export { commodityData, shopCartData, orderData };