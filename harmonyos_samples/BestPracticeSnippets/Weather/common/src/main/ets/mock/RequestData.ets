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


import FormDate from '../model/FormDate';
import { AirData, AirIndex, City, CityListData, Forecast, HeaderData, WeekWeather, SuitData } from '../model/Main';
import { MenuData } from '../model/MenuData';
import { cityData } from './CityInfo';
import { Aqi, weather } from './Mock';

const form = new FormDate(); // 时间格式修正
let uiContext = AppStorage.get('uiContext') as UIContext;
let resourceManager = uiContext.getHostContext()!.resourceManager;

// 获取天气类型对应的icon
function getTypeIcon(data: string) {

  switch (data) {
    case '晴':
      return $r('app.media.ic_weather_sunny');
    case 'Sunny':
      return $r('app.media.ic_weather_sunny');
    case 'Cloudy':
      return $r('app.media.ic_weather_cloudy');
    case '多云':
      return $r('app.media.ic_weather_cloudy');
    case '阴':
      return $r('app.media.ic_weather_yin');
    case '小雨':
      return $r('app.media.ic_weather_lightrain');
    case 'Rainy':
      return $r('app.media.ic_weather_lightrain');
    case '雷阵雨':
      return $r('app.media.ic_weather_lightrain');
    case 'Shower':
      return $r('app.media.ic_weather_lightrain');
    case '日落':
      return $r('app.media.ic_weather_sunset');
    case '中雨':
      return $r('app.media.ic_weather_rain');
    case 'Moderate rain':
      return $r('app.media.ic_weather_rain');
    case '大雨':
      return $r('app.media.ic_weather_rain');
    default:
      return $r('app.media.ic_weather_cloudy');
  }
}

function getUpdateTimes(): Resource[] {
  let times: Resource[] = [
    $r('app.string.one_hour'),
    $r('app.string.two_hour'),
    $r('app.string.four_hour'),
    $r('app.string.six_hour'),
    $r('app.string.twelve_hour'),
    $r('app.string.twenty_four_hour'),
    $r('app.string.not_update')
  ];
  return times;
}

function getMenuInfo(): MenuData[] {
  let data: MenuData[] =
    [
      new MenuData($r('app.string.urban_management'), 'pages/CityList'),
      new MenuData($r('app.string.update_interval'), '')
    ];
  return data;
}

// 获取首页顶部显示信息
function getHeaderDate(cityIndex: number, city?: City) {
  // 生成空气质量对象
  let air: AirData =
    new AirData(weather[cityIndex].aqi.air,
      resourceManager!.getStringSync(weather[cityIndex].aqi.airLevel as Resource));
  return new HeaderData(city ? city.name : resourceManager!.getStringSync(weather[cityIndex].city as Resource),
    weather[cityIndex].data[1].tem,
    resourceManager!.getStringSync(weather[cityIndex].data[1].wea as Resource), weather[cityIndex].data[1].tem1,
    weather[cityIndex].data[1].tem2,
    getTypeIcon(resourceManager!.getStringSync(weather[cityIndex].data[1].wea as Resource)), air,
    resourceManager!.getStringSync(weather[cityIndex].data[1].airTips as Resource));
}

function getCityList() {
  let cities: Array<City> = [];
  if (cityData) {
    cityData.forEach(item => {
      cities.push(new City(resourceManager!.getStringSync(item.name as Resource), item.temp, item.weather as Resource));
    })
  }
  return cities;
}

function addCity(city: City) {
  let cities: City[] | undefined = AppStorage.get('cityList');
  let cityIndex = cities!.length % 5;
  city.weather = weather[cityIndex].data[1].wea as Resource;
  city.temp = weather[cityIndex].data[1].tem;
  cities!.push(city);
  AppStorage.setOrCreate('cityList', cities);
}

function getCityListWeatherData() {
  let cityListWeatherData = new Array<CityListData>();
  // 生成空气质量对象

  let cities: City[] | undefined = AppStorage.get('cityList');
  if (cities === undefined || cities.length === 0) {
    cities = getCityList().splice(0, 2);
    AppStorage.setOrCreate('cityList', cities);
  }
  let titles: string[] = [];
  if (cities) {
    cities.forEach((item, cityIndex) => {
      let index = cityIndex % 5;
      let air: AirData = new AirData(weather[cityIndex].aqi.air,
        resourceManager!.getStringSync(weather[cityIndex].aqi.airLevel as Resource));
      cityListWeatherData.push(new CityListData(item.name, getHeaderDate(index, item), getHoursData(index),
        getWeekWeatherData(index), air, getAirIndexData(index), getLifeData(index)));
      titles.push('');
    })
  }
  AppStorage.setOrCreate('titleText', titles);
  return cityListWeatherData;
}


// 获取逐小时天气情况
function getHoursData(cityIndex: number) {
  let hoursData = new Array<Forecast>();
  weather[cityIndex].data[1].hours.forEach(item => {
    let time = item.hours.substring(0, item.hours.length - 1);
    hoursData.push(new Forecast(`${item.tem}°`, `${time}:00`, form.formTimeSlot(parseInt(time)),
      getTypeIcon(resourceManager!!.getStringSync(item.wea as Resource)),
      resourceManager!.getStringSync(item.win as Resource), item.winSpeed));
    if (item.hours.indexOf('17') !== -1) {
      hoursData.push(new Forecast('日落', '17:32', form.formTimeSlot(parseInt(time)), getTypeIcon('日落'),
        resourceManager!.getStringSync(item.win as Resource),
        item.winSpeed));
    }
  })
  return hoursData;
}

// 获取一周天气情况 根据获取到的数据格式化成需要的样式
function getWeekWeatherData(cityIndex: number) {
  let weekData = new Array<WeekWeather>();
  weather[cityIndex].data.forEach(item => {
    weekData.push(new WeekWeather(form.formMonthDay(item.date), resourceManager!.getStringSync(item.week as Resource),
      getTypeIcon(resourceManager!.getStringSync(item.wea as Resource)),
      resourceManager!.getStringSync(item.wea as Resource),
      resourceManager!.getStringSync(item.airLevel as Resource), item.tem1, item.tem2));
  })
  return weekData;
}

// 获取首页空气质量部分 右侧数据
function getAirIndexData(cityIndex: number) {
  let aqi: Aqi = weather[cityIndex].aqi;
  let indexDate: AirIndex[] = [
    new AirIndex('PM10', aqi.pm10),
    new AirIndex('PM2.5', aqi.pm25),
    new AirIndex('NO2', aqi.no2),
    new AirIndex('SO2', aqi.so2),
    new AirIndex('O2', aqi.o3),
    new AirIndex('CO', aqi.co)
  ];
  return indexDate;
}

// 获取生活指数 下方数据
function getLifeData(cityIndex: number) {
  let suitDate: SuitData[] = new Array<SuitData>();
  let iconList = [
    $r('app.media.icon_humidity'),
    $r('app.media.icon_cloth'),
    $r('app.media.icon_temperature'),
    $r('app.media.icon_sport'),
    $r('app.media.icon_breeze'),
    $r('app.media.icon_cold'),
    $r('app.media.icon_southerly'),
    $r('app.media.icon_uv')
  ];
  weather[cityIndex].data[1].index.forEach((item, index) => {
    suitDate.push(new SuitData(iconList[index], resourceManager!.getStringSync(item.title as Resource),
      resourceManager!.getStringSync(item.level as Resource)));
  })
  return suitDate;
}

// 根据天气类型 获取背景图片
function getSideBg(weatherType: string) {
  if (weatherType === '晴' || weatherType === 'Sunny') {
    return $r('app.media.weather_fine');
  } else if (weatherType === '阴' || weatherType === '多云' || weatherType === 'Cloudy') {
    return $r('app.media.weather_yin');
  } else {
    return $r('app.media.weather_rain');
  }
}


// 根据天气类型 获取背景图片
function getBg(weatherType: string) {
  if (weatherType === '晴') {
    return $r('app.media.sunny_bg');
  } else if (weatherType.includes('雨')) {
    return $r('app.media.rain_bg');
  } else {
    return $r('app.media.yin_bg');
  }
}

export {
  getUpdateTimes,
  getMenuInfo,
  getHeaderDate,
  getCityList,
  addCity,
  getCityListWeatherData,
  getHoursData,
  getWeekWeatherData,
  getAirIndexData,
  getLifeData,
  getSideBg,
  getBg
};