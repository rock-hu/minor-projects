/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

export interface TabType {
  id: number;
  name: ResourceStr;
}

export interface ImageType {
  id: number;
  url: string;
  type: number;
  resourceId: number;
}

export interface ResData {
  id: number;
  title: string;
  content: string;
  imagesUrl: Array<ImageType>;
  source: string;
}
export interface Refresh {
  isVisible: boolean;
  imageSrc: Resource;
  textValue: Resource;
  heightValue: number;
}