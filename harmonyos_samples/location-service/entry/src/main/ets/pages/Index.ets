/*
* Copyright (c) 2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import { promptAction } from '@kit.ArkUI';
import { AsyncCallback, BusinessError } from '@kit.BasicServicesKit';
import { MapComponent, mapCommon, map } from '@kit.MapKit';
import { geoLocationManager } from '@kit.LocationKit';
import { CommonConstants } from '../common/CommonConstants';

interface LocationInter {
  latitude: number,
  longitude: number
};

@Entry
@Component
struct LocationService {
  @State address: string = '';
  private mapController?: map.MapComponentController;
  private callback?: AsyncCallback<map.MapComponentController>;
  private marker?: map.Marker;
  private mapOption?: mapCommon.MapOptions;

  aboutToAppear(): void {
    geoLocationManager.off('locationChange', this.locationChange);
    let cameraPosition: mapCommon.CameraPosition = {
      target: { longitude: 113.886642, latitude: 22.878538 },
      zoom: 15
    };
    this.mapOption = {
      position: cameraPosition,
      scaleControlsEnabled: true,
    };
    this.callback = async (err, mapController) => {
      if (!err) {
        this.mapController = mapController;
      }
    };
  }

  getPreLocationPosition(): void {
    try {
      let location = geoLocationManager.getLastLocation();
      this.getAddress({
        latitude: location.latitude,
        longitude: location.longitude
      });
    } catch (err) {
      promptAction.showToast({
        message: JSON.stringify(err),
        duration: CommonConstants.TWO_THOUSAND
      });
    }
  }

  getLocationPosition(): void {
    let request: geoLocationManager.SingleLocationRequest = {
      locatingPriority: CommonConstants.PRIORITY_LOCATING_SPEED,
      locatingTimeoutMs: CommonConstants.TEN_THOUSAND
    };
    geoLocationManager.getCurrentLocation(request).then((location: geoLocationManager.Location) => {
      this.getAddress({
        latitude: location.latitude,
        longitude: location.longitude
      });
    }).catch((err: BusinessError) => {
      promptAction.showToast({
        message: JSON.stringify(err),
        duration: CommonConstants.TWO_THOUSAND
      });
    });
  }

  onLocationChange(): void {
    let request: geoLocationManager.ContinuousLocationRequest = {
      interval: 1,
      locationScenario: CommonConstants.NAVIGATION
    };
    try {
      geoLocationManager.on('locationChange', request, this.locationChange);
    } catch (err) {
      promptAction.showToast({
        message: JSON.stringify(err),
        duration: CommonConstants.TWO_THOUSAND
      });
    }
  }

  locationChange = (location: geoLocationManager.Location): void => {
    this.getAddress({
      latitude: location.latitude,
      longitude: location.longitude
    });
  }

  async getAddress(location: LocationInter) {
    try {
      let mapLocation: mapCommon.LatLng =
        await map.convertCoordinate(mapCommon.CoordinateType.WGS84, mapCommon.CoordinateType.GCJ02, {
          latitude: location.latitude,
          longitude: location.longitude
        });
      let cameraPosition: mapCommon.CameraPosition = {
        target: mapLocation,
        zoom: 15,
        tilt: 0,
        bearing: 0
      };
      let cameraUpdate = map.newCameraPosition(cameraPosition);
      this.mapController?.animateCamera(cameraUpdate, 100);
      let makerOptions: mapCommon.MarkerOptions = {
        position: mapLocation,
        icon: $r('app.media.point'),
        clickable: true,
        title: CommonConstants.MARKER_TITLE
      };
      this.marker = await this.mapController?.addMarker(makerOptions);
      let reverseGeocodeRequest: geoLocationManager.ReverseGeoCodeRequest = {
        locale: getContext(this).resourceManager.getStringSync($r('app.string.language')),
        latitude: location.latitude,
        longitude: location.longitude,
        maxItems: 1
      };
      geoLocationManager.getAddressesFromLocation(reverseGeocodeRequest, async (err, data) => {
        if (data) {
          this.address = data[0]?.placeName || '';
          this.marker?.setInfoWindowVisible(true)
          this.marker?.setSnippet(this.address)
        } else {
          promptAction.showToast({
            message: JSON.stringify(err),
            duration: CommonConstants.TWO_THOUSAND
          });
        }
      });
    } catch (error) {
      promptAction.showToast({
        message: JSON.stringify(error),
        duration: CommonConstants.TWO_THOUSAND
      });
    }
  }

  build() {
    Column() {
      Row() {
        if (canIUse('SystemCapability.Map.Core')) {
          MapComponent({
            mapOptions: this.mapOption,
            mapCallback: this.callback,
            customInfoWindow: this.customInfoWindow
          })
            .width(CommonConstants.ONE_HUNDRED_PERCENT)
            .height(CommonConstants.ONE_HUNDRED_PERCENT)
        }
      }
      .height(CommonConstants.EIGHTY_FIVE_PERCENT)
      .width(CommonConstants.ONE_HUNDRED_PERCENT)

      Column() {
        Row() {
          Text()
            .width($r('app.float.text_width'))
            .height($r('app.float.text_height'))
            .margin({ right: $r('app.float.text_margin_right') })
            .backgroundColor($r('app.color.green'))
            .borderRadius($r('app.float.text_border_radius'))
            .offset({ y: $r('app.float.text_offset') })
          Text() {
            Span($r('app.string.you_geocoded_location'))
              .fontSize($r('app.float.span_font_size'))
              .fontFamily('Medium')
              .fontColor($r('app.color.shallow_green'))
            Span(this.address)
              .fontSize($r('app.float.span_font_size'))
              .fontFamily('Medium')
              .fontColor($r('app.color.black'))
          }
        }
        .width(CommonConstants.ONE_HUNDRED_PERCENT)
        .padding($r('app.float.row_padding'))
        .alignItems(VerticalAlign.Top)

        Row() {
          Column() {
            Button($r('app.string.get_cache_location'))
              .onClick(() => {
                this.getPreLocationPosition();
              })
              .width(CommonConstants.ONE_HUNDRED_PERCENT)
              .height($r('app.float.button_height'))
              .margin({ right: $r('app.float.button_margin_right') })
          }
          .width(CommonConstants.FIFTY_PERCENT)

          Column() {
            Button($r('app.string.get_current_location'))
              .onClick(() => {
                this.getLocationPosition();
              })
              .width(CommonConstants.ONE_HUNDRED_PERCENT)
              .height($r('app.float.button_height'))
              .backgroundColor($r('app.color.black_0D'))
              .fontColor($r('app.color.blue'))
              .margin({ left: $r('app.float.button_margin_left') })
          }
          .width(CommonConstants.FIFTY_PERCENT)
        }
        .margin({
          left: $r('app.float.column_margin_left'),
          right: $r('app.float.column_margin_right')
        })

        Column() {
          Button($r('app.string.continuous_positioning'))
            .onClick(() => {
              this.onLocationChange();
            })
            .width(CommonConstants.ONE_HUNDRED_PERCENT)
            .height($r('app.float.button_height'))
            .backgroundColor($r('app.color.black_0D'))
            .fontSize($r('app.float.button_font_size'))
            .fontFamily('Medium')
            .fontColor($r('app.color.blue'))
        }
        .width(CommonConstants.ONE_HUNDRED_PERCENT)
        .padding({
          left: $r('app.float.column_padding_left'),
          right: $r('app.float.column_padding_right'),
          top: $r('app.float.column_padding_top'),
          bottom: $r('app.float.column_padding_bottom')
        })

      }
      .width(CommonConstants.ONE_HUNDRED_PERCENT)
      .borderRadius({
        topLeft: $r('app.float.column_border_radius_top'),
        topRight: $r('app.float.column_border_radius_top')
      })
      .backgroundColor($r('app.color.white'))
      .position({ bottom: $r('app.float.column_position_bottom') })
    }
    .height(CommonConstants.ONE_HUNDRED_PERCENT)
  }

  @BuilderParam customInfoWindow: (params: map.MarkerDelegate) => void = this.customInfoWindowBuilder;

  @Builder
  customInfoWindowBuilder(params: map.MarkerDelegate) {
    if (params.marker) {
      Column() {
        Text(params.marker.getTitle())
          .width($r('app.float.text_custom_info_title_width'))
          .height($r('app.float.text_custom_info_title_height'))
          .backgroundColor($r('app.color.green'))
          .borderRadius({
            topLeft: $r('app.float.text_custom_info_title_border_radius_top'),
            topRight: $r('app.float.text_custom_info_title_border_radius_top')
          })
          .padding({
            top: $r('app.float.text_custom_info_title_padding_top'),
            bottom: $r('app.float.text_custom_info_title_padding_bottom'),
            left: $r('app.float.text_custom_info_title_padding_left')
          })
          .fontSize($r('app.float.text_custom_info_title_font_size'))
          .fontColor($r('app.color.white_e6'))
          .lineHeight($r('app.float.text_custom_info_title_line_height'))
          .textAlign(TextAlign.Start)
        Text(params.marker.getSnippet())
          .width($r('app.float.text_custom_info_snippet_width'))
          .backgroundColor($r('app.color.white'))
          .borderRadius({
            bottomLeft: $r('app.float.text_custom_info_snippet_border_radius_top'),
            bottomRight: $r('app.float.text_custom_info_snippet_border_radius_top')
          })
          .padding({
            top: $r('app.float.text_custom_info_snippet_padding_top'),
            bottom: $r('app.float.text_custom_info_snippet_padding_bottom'),
            left: $r('app.float.text_custom_info_snippet_padding_left'),
            right: $r('app.float.text_custom_info_snippet_padding_right')
          })
          .fontSize($r('app.float.text_custom_info_snippet_font_size'))
          .fontColor($r('app.color.black_e6'))
          .lineHeight($r('app.float.text_custom_info_snippet_line_height'))
          .textAlign(TextAlign.Start)
      }
    }
  }
}