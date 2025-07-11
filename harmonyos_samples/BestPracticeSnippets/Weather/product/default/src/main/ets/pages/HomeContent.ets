/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

// [Start HomeContent]
import AirQuality from './AirQuality';
import HoursWeather from './HoursWeather';
import IndexHeader from './IndexHeader';
import IndexEnd from './IndexEnd';
import LifeIndex from './LifeIndex';
import MultidayWeather from './MultidayWeather';
import SunCanvas from './SunCanvas';
import { CityListData, Style } from 'common';

@Component
export default struct HomeContent {
  @Prop showSideBar: boolean;
  @StorageLink('titleText') titleText: string[] = [];
  @StorageLink('swiperIndex') swiperIndex: number = 0;
  @State headerOpacity: number = 1;
  cityListData?: CityListData;
  index: number = 1;
  scroller: Scroller = new Scroller();

  build() {
    Scroll(this.scroller) {
      GridRow({
        columns: {
          xs: 4,
          sm: 4,
          md: 8,
          lg: this.showSideBar ? 8 : 12
        },
        gutter: { x: Style.GRID_GUTTER, y: Style.GRID_GUTTER },
        breakpoints: { reference: BreakpointsReference.WindowSize }
      }) {
        // Weather overview.
        GridCol({
          span: {
            xs: 4,
            sm: 4,
            md: 8,
            lg: this.showSideBar ? 8 : 12
          },
          order: 0
        }) {
          IndexHeader({ headerDate: this.cityListData!.header, index: this.index })
            .opacity(this.headerOpacity)
        }

        // Hourly weather.
        GridCol({
          span: {
            xs: 4,
            sm: 4,
            md: 8,
            lg: 8
          },
          order: 1
        }) {
          HoursWeather({ hoursData: this.cityListData!.hoursData })
        }

        // Daily weather.
        GridCol({
          span: 4,
          order: {
            xs: 2,
            sm: 2,
            md: 2,
            lg: this.showSideBar ? 2 : 3
          }
        },) {
          MultidayWeather({ weekData: this.cityListData!.weekData })
        }

        // Air quality.
        GridCol({
          span: 4,
          order: {
            xs: 3,
            sm: 3,
            md: 3,
            lg: this.showSideBar ? 3 : 2
          }
        }) {
          AirQuality({ airData: this.cityListData!.airData, airIndexData: this.cityListData!.airIndex })
        }

        // Living index.
        GridCol({ span: 4, order: 4 }) {
          LifeIndex({ lifeData: this.cityListData!.suitDate })
        }

        // Sun canvas.
        GridCol({ span: 4, order: 5 }) {
          SunCanvas()
        }

        // Index end.
        GridCol({
          span: {
            xs: 4,
            sm: 4,
            md: 8,
            lg: this.showSideBar ? 8 : 12
          },
          order: 6
        }) {
          IndexEnd()
        }
      }
    }
    // [StartExclude HomeContent]
    .id('homeContent' + this.index)
    .width('100%')
    .layoutWeight(1)
    .scrollBar(BarState.Off)
    .onWillScroll(() => {
      this.headerOpacity = 1 - (this.scroller.currentOffset().yOffset - 45) / 100;
      if (this.scroller.currentOffset().yOffset > 45) {
        this.titleText[this.index] = this.cityListData!.header.city;
        AppStorage.setOrCreate('titleText', this.titleText);
      } else if (this.scroller.currentOffset().yOffset < 45) {
        this.titleText[this.index] = '';
        AppStorage.setOrCreate('titleText', this.titleText);
      }
    })
    // [EndExclude HomeContent]
  }
}
// [End HomeContent]