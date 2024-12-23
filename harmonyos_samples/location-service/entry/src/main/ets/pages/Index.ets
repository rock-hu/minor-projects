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
import { hilog } from '@kit.PerformanceAnalysisKit';
import { common, wantAgent } from '@kit.AbilityKit';
import { backgroundTaskManager } from '@kit.BackgroundTasksKit';
import { connection } from '@kit.NetworkKit';
import { CommonConstants } from '../common/CommonConstants';
import { LocationInter } from '../model/LocationInter';

const TAG = '[Index]';

@Entry
@Component
struct LocationService {
  @State address: string = '';
  @State isBackgroundRunning: boolean = false;
  @State isOnLocationChange: boolean = false;
  @State isCacheLocation: boolean = false;
  @StorageProp('bottomRectHeight') bottomRectHeight: number = 0;
  private mapController?: map.MapComponentController;
  private callback?: AsyncCallback<map.MapComponentController>;
  private marker?: map.Marker;
  private mapOption?: mapCommon.MapOptions;

  aboutToAppear(): void {
    if (!this.judgeHasNet()) {
      promptAction.showToast({
        message: $r('app.string.internet_switch'),
        duration: 2000
      });
    }
    let cameraPosition: mapCommon.CameraPosition = {
      target: { longitude: 113.886642, latitude: 22.878538 },
      zoom: 15
    };
    this.mapOption = {
      position: cameraPosition,
      scaleControlsEnabled: true
    };
    this.callback = async (err, mapController) => {
      if (!err) {
        this.mapController = mapController;
      }
    };
  }

  aboutToDisappear(): void {
    if (this.isBackgroundRunning) {
      this.stopContinuousTask();
    } else {
      if (this.isOnLocationChange) {
        this.offLocationChange();
      }
    }
  }

  /**
   * Get cache location information
   */
  getPreLocationPosition(): void {
    try {
      let location = geoLocationManager.getLastLocation();
      this.getAddress({
        latitude: location.latitude,
        longitude: location.longitude
      });
    } catch (err) {
      hilog.error(0x0000, TAG, `getPreLocationPosition failed, code: ${err.code}, message: ${err.message}`);
      this.locationFailedAlert(err.code);
    }
  }

  /**
   * Get current location information
   */
  getLocationPosition(): void {
    let request: geoLocationManager.SingleLocationRequest = {
      locatingPriority: CommonConstants.PRIORITY_LOCATING_SPEED,
      locatingTimeoutMs: CommonConstants.LOCATING_TIMEOUT_MS
    };
    geoLocationManager.getCurrentLocation(request).then((location: geoLocationManager.Location) => {
      this.getAddress({
        latitude: location.latitude,
        longitude: location.longitude
      })
    }).catch((err: BusinessError) => {
      hilog.error(0x0000, TAG, `getCurrentLocationPosition failed, code: ${err.code}, message: ${err.message}`);
      this.locationFailedAlert(err.code);
    });
  }

  /**
   * Start position change listening
   */
  onLocationChange(): void {
    let request: geoLocationManager.ContinuousLocationRequest = {
      interval: 1,
      locationScenario: CommonConstants.NAVIGATION
    };
    try {
      geoLocationManager.on('locationChange', request, this.locationChange);
      if (!this.judgeHasNet()) {
        promptAction.showToast({
          message: $r('app.string.internet_switch'),
          duration: 2000
        });
        this.marker?.setInfoWindowVisible(false);
        this.address = '';
      }
    } catch (err) {
      hilog.error(0x0000, TAG, `onLocationChange failed, code: ${err.code}, message: ${err.message}`);
      this.locationFailedAlert(err.code);
    }
  }

  /**
   * Stop position change listening
   */
  offLocationChange(): void {
    try {
      geoLocationManager.off('locationChange', this.locationChange);
    } catch (err) {
      hilog.error(0x0000, TAG, `offLocationChange failed, code: ${err.code}, message: ${err.message}`);
    }
  }

  locationChange = (location: geoLocationManager.Location): void => {
    this.getAddress({
      latitude: location.latitude,
      longitude: location.longitude
    });
  }

  /**
   * Reverse geocoding and drawing the marker on the map
   * @param location
   */
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
      if (this.marker === undefined) {
        this.marker = await this.mapController?.addMarker(makerOptions);
      } else {
        this.marker.setPosition(mapLocation);
      }
      let reverseGeocodeRequest: geoLocationManager.ReverseGeoCodeRequest = {
        locale: getContext(this).resourceManager.getStringSync($r('app.string.language')),
        latitude: location.latitude,
        longitude: location.longitude,
        maxItems: 1
      };
      geoLocationManager.getAddressesFromLocation(reverseGeocodeRequest, async (err, data) => {
        if (data) {
          this.address = data[0]?.placeName || '';
          this.marker?.setInfoWindowVisible(true);
          this.marker?.setSnippet(this.address);
          hilog.info(0x0000, TAG, `The address is: ${this.address}`);
          if (this.isBackgroundRunning || this.isOnLocationChange) {
            this.isCacheLocation = false;
          }
        } else {
          hilog.error(0x0000, TAG, `getAddressesFromLocation failed, code: ${err.code}, message: ${err.message}`);
          if (!this.isOnLocationChange && !this.isBackgroundRunning) {
            this.locationFailedAlert(err.code);
          }
        }
      });
    } catch (error) {
      hilog.error(0x0000, TAG, `getAddress failed, code: ${error.code}, message: ${error.message}`);
    }
  }

  /**
   * Start Background Location
   */
  startContinuousTask(): void {
    let context = getContext(this) as common.UIAbilityContext;
    if (!context) {
      return;
    }
    let wantAgentInfo: wantAgent.WantAgentInfo = {
      wants: [
        {
          bundleName: context.abilityInfo.bundleName,
          abilityName: context.abilityInfo.name
        }
      ],
      operationType: wantAgent.OperationType.START_ABILITY,
      requestCode: 1,
      wantAgentFlags: [wantAgent.WantAgentFlags.UPDATE_PRESENT_FLAG]
    };

    wantAgent.getWantAgent(wantAgentInfo).then((wantAgentObj) => {
      backgroundTaskManager.startBackgroundRunning(context,
        backgroundTaskManager.BackgroundMode.LOCATION, wantAgentObj).then(() => {
        this.onLocationChange();
        hilog.info(0x0000, TAG, 'startBackgroundRunning succeeded');
      }).catch((err: BusinessError) => {
        hilog.error(0x0000, TAG, `startBackgroundRunning failed, cause:  ${JSON.stringify(err)}`);
      });
    });
  }

  /**
   * Stop Background Location
   */
  stopContinuousTask(): void {
    let context = getContext(this) as common.UIAbilityContext;
    if (!context) {
      return;
    }
    backgroundTaskManager.stopBackgroundRunning(context).then(() => {
      if (!this.isOnLocationChange) {
        this.offLocationChange();
      }
      hilog.info(0x0000, TAG, 'stopBackgroundRunning succeeded');
    }).catch((err: BusinessError) => {
      hilog.error(0x0000, TAG, `stopBackgroundRunning failed, cause:  ${JSON.stringify(err)}`);
    });
  }

  locationFailedAlert(errCode: number) {
    if (errCode === CommonConstants.LOCATION_SWITCH_OFF) {
      promptAction.showToast({
        message: $r('app.string.location_switch'),
        duration: 2000
      });
    } else if (errCode === CommonConstants.LOCATION_FAILED) {
      promptAction.showToast({
        message: $r('app.string.internet_switch'),
        duration: 2000
      });
    } else if (errCode === CommonConstants.REVERSE_GEOCODING_FAILED) {
      promptAction.showToast({
        message: $r('app.string.internet_switch'),
        duration: 2000
      });
    } else if (errCode === CommonConstants.WIFI_BLUETOOTH_OFF) {
      promptAction.showToast({
        message: $r('app.string.wifi_bluetooth_switch'),
        duration: 2000
      });
    }
    this.marker?.setInfoWindowVisible(false);
    this.address = '';
  }

  judgeHasNet(): boolean {
    try {
      let netHandle = connection.getDefaultNetSync();
      if (!netHandle || netHandle.netId === 0) {
        return false;
      }
      let netCapability = connection.getNetCapabilitiesSync(netHandle);
      let cap = netCapability.networkCap || [];
      if (cap.includes(connection.NetCap.NET_CAPABILITY_VALIDATED)) {
        return true;
      } else {
        return false;
      }
    } catch (err) {
      hilog.error(0x0000, TAG, `JudgeHasNet failed, code: ${err.code}, message: ${err.message}`);
    }
    return false;
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
            .width('100%')
            .height('100%')
        }
      }
      .height('83%')
      .width('100%')

      Column() {
        Row() {
          Text()
            .width(8)
            .height(8)
            .margin({ right: 7 })
            .backgroundColor($r('app.color.green'))
            .borderRadius(4)
            .offset({ y: 6 })
          Text() {
            Span(this.isCacheLocation ? $r('app.string.cache_geocoded_location') :
            $r('app.string.current_geocoded_location'))
              .fontSize(14)
              .lineHeight(19)
              .fontFamily('Medium')
              .fontColor($r('app.color.shallow_green'))
            Span(this.address)
              .fontSize(14)
              .lineHeight(19)
              .fontFamily('Medium')
              .fontColor($r('app.color.black'))
          }
          .margin({ right: 16 })
        }
        .width('100%')
        .padding({
          top: 24,
          bottom: 16,
          left: 16,
          right: 16
        })
        .margin({
          left: 16,
          right: 16
        })
        .constraintSize({ maxWidth: '100%' })
        .alignItems(VerticalAlign.Top)

        Column() {
          Row() {
            Text($r('app.string.continuous_positioning'))
              .fontSize(16)
              .fontWeight(FontWeight.Medium)

            Toggle({ type: ToggleType.Switch })
              .onChange((isOn: boolean) => {
                if (isOn) {
                  this.isOnLocationChange = true;
                  this.onLocationChange();
                } else {
                  this.isOnLocationChange = false;
                  if (!this.isBackgroundRunning) {
                    this.offLocationChange();
                  }
                }
              })
          }
          .height(48)
          .width('100%')
          .constraintSize({ maxWidth: '100%' })
          .justifyContent(FlexAlign.SpaceBetween)
          .padding({
            left: 16,
            right: 16
          })

          Row() {
            Text($r('app.string.background_positioning'))
              .fontSize(16)
              .fontWeight(FontWeight.Medium)

            Toggle({ type: ToggleType.Switch })
              .onChange((isOn: boolean) => {
                if (isOn) {
                  this.isBackgroundRunning = true;
                  this.startContinuousTask();
                } else {
                  this.isBackgroundRunning = false;
                  this.stopContinuousTask();
                }
              })
          }
          .height(48)
          .width('100%')
          .constraintSize({ maxWidth: '100%' })
          .justifyContent(FlexAlign.SpaceBetween)
          .padding({
            left: 16,
            right: 16
          })
          .margin({
            bottom: 24
          })

          Button($r('app.string.get_cache_location'))
            .onClick(() => {
              this.getPreLocationPosition();
              this.isCacheLocation = true;
            })
            .width('100%')
            .height(40)
            .backgroundColor($r('app.color.blue'))
            .fontColor($r('app.color.white'))
            .fontWeight(FontWeight.Medium)
            .margin({
              bottom: 12
            })

          Button($r('app.string.get_current_location'))
            .onClick(() => {
              this.getLocationPosition();
              this.isCacheLocation = false;
            })
            .width('100%')
            .height(40)
            .backgroundColor($r('app.color.blue'))
            .fontColor($r('app.color.white'))
            .fontWeight(FontWeight.Medium)
        }
        .width('100%')
        .constraintSize({ maxWidth: '100%' })
        .margin({
          left: 16,
          right: 16,
          bottom: 16
        })
      }
      .width('100%')
      .borderRadius({
        topLeft: 32,
        topRight: 32
      })
      .backgroundColor($r('sys.color.comp_background_list_card'))
      .position({ bottom: 0 })
    }
    .height('100%')
    .padding({
      bottom: px2vp(this.bottomRectHeight)
    })
  }

  @BuilderParam customInfoWindow: (params: map.MarkerDelegate) => void = this.customInfoWindowBuilder;

  @Builder
  customInfoWindowBuilder(params: map.MarkerDelegate) {
    if (params.marker) {
      Column() {
        Text(params.marker.getTitle())
          .width(180)
          .height(24)
          .backgroundColor($r('app.color.green'))
          .borderRadius({
            topLeft: 12,
            topRight: 12
          })
          .padding({
            top: 4,
            bottom: 4,
            left: 12
          })
          .fontSize(12)
          .fontColor($r('app.color.white_e6'))
          .lineHeight(16)
          .textAlign(TextAlign.Start)
        Text(params.marker.getSnippet())
          .width(180)
          .backgroundColor($r('app.color.white'))
          .borderRadius({
            bottomLeft: 12,
            bottomRight: 12
          })
          .padding({
            top: 4,
            bottom: 6,
            left: 12,
            right: 8
          })
          .fontSize(14)
          .fontColor($r('app.color.black_e6'))
          .lineHeight(19)
          .textAlign(TextAlign.Start)
      }
    }
  }
}