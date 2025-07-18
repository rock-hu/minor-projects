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

// 预测 气象列表
class Forecast {
  public degree: string;
  public date: string;
  public desc: string;
  public icon: Resource | string;
  public win: string;
  public winSpeed: string;

  constructor(degree: string, date: string, desc: string, icon: Resource | string, win: string, winSpeed: string) {
    this.degree = degree;
    this.date = date;
    this.desc = desc;
    this.icon = icon;
    this.win = win;
    this.winSpeed = winSpeed;
  }
}

// 一周天气
class WeekWeather {
  public date: string;
  public week: string;
  public icon: Resource | string;
  public weather: string;
  public airLevel: string;
  public max: string;
  public min: string;

  constructor(date: string, week: string, icon: Resource | string, weather: string, airLevel: string, max: string,
    min: string) {
    this.date = date;
    this.week = week;
    this.icon = icon;
    this.weather = weather;
    this.airLevel = airLevel;
    this.max = max;
    this.min = min;
  }
}

// 空气质量
class AirIndex {
  public index: string;
  public figure: string;

  constructor(index: string, figure: string) {
    this.index = index;
    this.figure = figure;
  }
}

// 舒适度
class SuitData {
  public src: Resource;
  public desc: string;
  public value: string;

  constructor(src: Resource, desc: string, value: string) {
    this.src = src;
    this.desc = desc;
    this.value = value;
  }
}

// canvas里数据
class AirData {
  public airQuality: string;
  public airDesc: string;

  constructor(airQuality: string, airDesc: string) {
    this.airQuality = airQuality;
    this.airDesc = airDesc;
  }
}

class HeaderData {
  public city: string;
  public temp: string;
  public weatherType: string;
  public max: string;
  public min: string;
  public icon: Resource | string;
  public airData: AirData;
  public airTips: string;

  constructor(city: string, temp: string, weatherType: string, max: string, min: string, icon: Resource | string,
    airData: AirData,
    airTips: string) {
    this.city = city;
    this.temp = temp;
    this.weatherType = weatherType;
    this.max = max;
    this.min = min;
    this.icon = icon;
    this.airData = airData;
    this.airTips = airTips;
  }
}

// city里数据
class City {
  public name: string;
  public temp: string;
  public weather: string;

  constructor(name: string, temp: string, weather: string) {
    this.name = name;
    this.temp = temp;
    this.weather = weather;
  }
}

class CityListData {
  public city: string;
  public header: HeaderData;
  public hoursData: Forecast[];
  public weekData: WeekWeather[];
  public airData: AirData;
  public airIndex: AirIndex[];
  public suitDate: SuitData[];

  constructor(city: string, header: HeaderData, hoursData: Forecast[], weekData: WeekWeather[], airData: AirData,
    airIndex: AirIndex[], suitDate: SuitData[]) {
    this.city = city;
    this.header = header;
    this.hoursData = hoursData;
    this.weekData = weekData;
    this.airData = airData;
    this.airIndex = airIndex;
    this.suitDate = suitDate;
  }
}


export { HeaderData, CityListData, Forecast, AirIndex, SuitData, WeekWeather, AirData, City }