/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

import CustomRefreshLoadLayout from './CustomRefreshLoadLayout';
import { CustomRefreshLoadLayoutClass } from '../viewmodel/NewsData';

/**
 * The refresh layout component.
 */
@Component
export default struct RefreshLayout {
  @ObjectLink @Watch('newCustom') refreshLayoutClass: CustomRefreshLoadLayoutClass;
  @State customRefreshLoadClass: CustomRefreshLoadLayoutClass =
    new CustomRefreshLoadLayoutClass(this.refreshLayoutClass.isVisible, this.refreshLayoutClass.imageSrc,
      this.refreshLayoutClass.textValue,
      this.refreshLayoutClass.heightValue);

  newCustom() {
    this.customRefreshLoadClass =
      new CustomRefreshLoadLayoutClass(this.refreshLayoutClass.isVisible, this.refreshLayoutClass.imageSrc,
        this.refreshLayoutClass.textValue,
        this.refreshLayoutClass.heightValue);
  }

  build() {
    Column() {
      if (this.refreshLayoutClass.isVisible) {
        CustomRefreshLoadLayout({ customRefreshLoadClass: this.customRefreshLoadClass })
      }
    }
  }
}