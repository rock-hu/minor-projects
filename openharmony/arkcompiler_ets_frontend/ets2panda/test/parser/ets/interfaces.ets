/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

interface I {
  RED: int;
  BLUE:int;
}

interface I0 {
  private nop(): void { I0.nopnop(); }
  nopnop(): void {}
  nopnopnop(): void {}
}

interface I1 extends I, I0 {
}

/* @@? 22:28 Error TypeError: 'nopnop' is an instance property of 'I0' */
