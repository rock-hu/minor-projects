/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

import { MapComponent, mapCommon, map } from '@kit.MapKit';
import { AsyncCallback } from '@kit.BasicServicesKit';

@Entry
@Component
struct BasicMapDemo {
  private mapOptions?: mapCommon.MapOptions;
  private callback?: AsyncCallback<map.MapComponentController>;
  private mapController?: map.MapComponentController;

  aboutToAppear(): void {
    // Map initialization parameters used to set the coordinates of the map center point and zoom level.
    let target: mapCommon.LatLng = {
      latitude: 39.9181,
      longitude: 116.3970193
    };
    let cameraPosition: mapCommon.CameraPosition = {
      target: target,
      zoom: 15
    };
    this.mapOptions = {
      position: cameraPosition
    };

    // Map initialization callback.
    this.callback = async (err, mapController) => {
      if (!err) {
        this.mapController = mapController;
        // Marker initialization parameters.
        let markerOptions: mapCommon.MarkerOptions = {
          position: {
            latitude: 39.9181,
            longitude: 116.3970193
          }
        };
        // Create a default marker icon.
        await this.mapController?.addMarker(markerOptions);
      }
    }
  }

  build() {
    Stack() {
      // Call the MapComponent component to initialize the map.
      MapComponent({ mapOptions: this.mapOptions, mapCallback: this.callback }).width('100%').height('100%');
    }.height('100%')
  }
}
