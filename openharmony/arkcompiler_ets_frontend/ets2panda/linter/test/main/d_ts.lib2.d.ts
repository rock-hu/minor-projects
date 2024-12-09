/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

// issue 13043
declare type Length = string | number | Resource;
declare interface Resource {
  readonly id: number;
  readonly type: number;
}
declare type Padding = {
  top?: Length;
  right?: Length;
  bottom?: Length;
  left?: Length;
}
export type Margin = Padding;

// issue 13048
export interface BusinessError<T = void> extends Error {
  code: number;
  data?: T;
}
export interface AsyncCallback<T, E = void> {
  (err: BusinessError<E>, data: T): void;
}
export interface AsyncCallback2<T> {
  (err: { v: number, [v: string]: number }, data: T): void;
}
