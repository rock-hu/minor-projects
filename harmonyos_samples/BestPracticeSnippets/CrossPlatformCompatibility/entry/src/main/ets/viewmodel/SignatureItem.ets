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

export class SignatureItem {
  name: Resource = $r('app.string.empty');
  type: string = '';
  keyGenAlg: string = '';
  isSegment: boolean = false;

  constructor(name: Resource, type: string, keyGenAlg: string, isSegment: boolean) {
    this.name = name;
    this.type = type;
    this.keyGenAlg = keyGenAlg;
    this.isSegment = isSegment;
  }
}