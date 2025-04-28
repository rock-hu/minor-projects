/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import { connection } from '@kit.NetworkKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Builder
export function GetNetworkConnectionInfoBuilder() {
  GetNetworkConnectionInfo()
}

@Component
export struct GetNetworkConnectionInfo {
  @State text: ResourceStr = '';
  @State textArea: string = '';

  // DocsCode 1
  getNetworkConnectionType() {
    try {
      let netHandle = connection.getDefaultNetSync();
      if (!netHandle || netHandle.netId === 0) {
        return;
      }
      let netCapability = connection.getNetCapabilitiesSync(netHandle);
      hilog.info(0x0000, 'Sample', 'bearerTypes: %{public}s', JSON.stringify(netCapability.bearerTypes));
      // DocsDot
      this.textArea = 'bearerTypes:' + JSON.stringify(netCapability.bearerTypes);
      // DocsDot
    } catch (e) {
      let err = e as BusinessError;
      hilog.error(0x0000, 'Sample', 'errCode: %{public}s, errMessage: %{public}s', (err as BusinessError).code,
        (err as BusinessError).message);
    }
  }

  // DocsCode 1

  // DocsCode 2
  judgeHasNet(): boolean {
    try {
      let netHandle = connection.getDefaultNetSync();
      if (!netHandle || netHandle.netId === 0) {
        return false;
      }
      let netCapability = connection.getNetCapabilitiesSync(netHandle);
      let cap = netCapability.networkCap || [];
      if (cap.includes(connection.NetCap.NET_CAPABILITY_VALIDATED)) {
        // connection.NetCap.NET_CAPABILITY_VALIDATED，The value indicates that the network is normal and HTTP and HTTPS requests can be initiated。
        // The network information changes and the network is available
        // DocsDot
        this.textArea = 'true';
        // DocsDot
        return true;
      } else {
        // The network information changes and the network is unavailable.
        // DocsDot
        this.textArea = 'false';
        // DocsDot
        return false;
      }
    } catch (e) {
      let err = e as BusinessError;
      hilog.error(0x0000, 'Sample', 'JudgeHasNet: %{public}s', JSON.stringify(err));
    }
    return false;
  }

  // DocsCode 2

  // DocsCode 3
  getRouteInfoAndLinkAddress() {
    connection.getDefaultNet().then((netHandle: connection.NetHandle) => {
      if (!netHandle || netHandle.netId === 0) {
        // If there is no default network, the netid of the obtained netHandler is 0, which is an exception. You can add some processing mechanisms as required.
        return;
      }
      connection.getConnectionProperties(netHandle, (error: BusinessError, data: connection.ConnectionProperties) => {
        if (error) {
          hilog.error(0x0000, 'Sample', 'errCode: %{public}s, errMessage: %{public}s', error.code, error.message);
          return;
        }
        hilog.info(0x0000, 'Sample', 'Succeeded to get data: %{public}s', JSON.stringify(data));
        let routeInfo = data.routes;
        let linkAddress = data.linkAddresses;
        // DocsDot
        this.textArea =
          'routeInfo is: ' + JSON.stringify(routeInfo) + '\n' + 'linkAddress is: ' + JSON.stringify(linkAddress);
        // DocsDot
      })
    });
  }

  // DocsCode 3

  @Builder
  NavDestinationTitle() {
    Column() {
      Text($r('app.string.query_Network_connection_information'))
        .fontSize(20)
        .lineHeight(40)
        .fontWeight(700)
        .width('100%')
        .padding({ left: 12 })
    }
    .width('100%')
  }

  build() {
    NavDestination() {
      Column() {
        Column() {
          Text(this.text)
            .margin({ bottom: 12 })
            .textAlign(TextAlign.Start)
            .width('100%')
            .height(20)
          TextArea({ text: this.textArea })
            .width('100%')
            .height('45%')
            .focusable(false)
        }

        Column({ space: 12 }) {
          Button($r('app.string.get_current_network_connection_type'))
            .onClick(() => {
              this.text = $r('app.string.current_network_connection_type');
              this.getNetworkConnectionType();
            })
            .width('100%')
            .backgroundColor('#0A59F7')
          Button($r('app.string.check_network_is_available'))
            .onClick(() => {
              this.text = $r('app.string.network_is_available');
              this.judgeHasNet();
            })
            .width('100%')
            .backgroundColor('#0A59F7')
          Button($r('app.string.query_route_and_link_information'))
            .onClick(() => {
              this.text = $r('app.string.route_and_link_information');
              this.getRouteInfoAndLinkAddress();
            })
            .width('100%')
            .backgroundColor('#0A59F7')
        }
      }
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .padding(16)
    }
    .title(this.NavDestinationTitle())
  }
}