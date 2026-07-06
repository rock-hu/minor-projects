/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

export const cityData: CityInfo[] = [
  {
    "name": $r('app.string.Beijing'),
    "temp": "32",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Shanghai'),
    "temp": "28",
    "weather": $r('app.string.Cloudy')
  },
  {
    "name": $r('app.string.Guangzhou'),
    "temp": "35",
    "weather": $r('app.string.Rainy')
  },
  {
    "name": $r('app.string.Tianjin'),
    "temp": "35",
    "weather": $r('app.string.Rainy')
  },
  {
    "name": $r('app.string.Wuhan'),
    "temp": "26",
    "weather": $r('app.string.Cloudy')
  },
  {
    "name": $r('app.string.Shenyang'),
    "temp": "25",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Chongqing'),
    "temp": "27",
    "weather": $r('app.string.Rainy')
  },
  {
    "name": $r('app.string.Hangzhou'),
    "temp": "35",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Nanjing'),
    "temp": "25",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Harbin'),
    "temp": "25",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Changchun'),
    "temp": "28",
    "weather": $r('app.string.Shower')
  },
  {
    "name": $r('app.string.Hohhot'),
    "temp": "25",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Shijiazhuang'),
    "temp": "25",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Yinchuan'),
    "temp": "25",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Urumqi'),
    "temp": "25",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Lhasa'),
    "temp": "35",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Xining'),
    "temp": "25",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Xian'),
    "temp": "25",
    "weather": $r('app.string.Sunny')
  },
  {
    "name": $r('app.string.Lanzhou'),
    "temp": "25",
    "weather": $r('app.string.Cloudy')
  },
  {
    "name": $r('app.string.Taiyuan'),
    "temp": "25",
    "weather": $r('app.string.Cloudy')
  },
  {
    "name": $r('app.string.Kunming'),
    "temp": "25",
    "weather": $r('app.string.Rainy')
  },
  {
    "name": $r('app.string.Nanning'),
    "temp": "20",
    "weather": $r('app.string.Sunny')
  }
];

export class CityInfo {
  name: ResourceStr = '';
  temp: string = '';
  weather: ResourceStr = '';
}