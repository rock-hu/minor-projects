/*
* Copyright (C) 2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
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

/*
* 最佳实践: 应用切面编程设计
*/

// [Start fragment1]
import { geoLocationManager } from "@kit.LocationKit";
// [End fragment1]
// [Start base]
// base.ets
export class Base {
  foo() {
    console.log('hello');
  }
}
// [End base]

// [Start base1]
// [Start fragment1]
export class Base1 {
  getCurrentLocation() {
    return geoLocationManager.getCurrentLocation();
  }
}
// [End fragment1]
// [End base1]
