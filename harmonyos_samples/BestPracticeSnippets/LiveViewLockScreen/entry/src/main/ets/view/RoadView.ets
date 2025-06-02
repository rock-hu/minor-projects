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

@Component
export struct RoadView {
  @Prop laneNum: number = 1;

  build() {
    Stack() {
      Row() {
        if (this.laneNum === 1) {
          Image($r('app.media.traffic_left'))
            .objectFit(ImageFit.Contain)
        } else {
          Image($r('app.media.traffic_right'))
            .objectFit(ImageFit.Contain)
        }
      }
      .width('80%')
      .height('80%')
    }
    .backgroundImage($r("app.media.traffic_background"))
    .backgroundImageSize(ImageSize.Cover)
    .backgroundImagePosition(Alignment.Center)
    .height('101%')
    .width('101%')
  }
}