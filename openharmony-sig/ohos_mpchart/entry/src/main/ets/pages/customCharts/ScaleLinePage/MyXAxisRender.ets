/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

import { Utils, XAxisRenderer } from '@ohos/mpchart';

export default class MyXAxisRender extends XAxisRenderer{
  protected drawGridLine(c: CanvasRenderingContext2D, x: number, y: number): void {
    Utils.resetContext2DWithoutFont(c, this.mGridPaint);
    c.beginPath();
    let bottom = this.mViewPortHandler?.contentBottom()??0;
    c.moveTo(x, bottom);
    c.lineTo(x, bottom - 10);
    c.stroke();
    c.closePath();
  }
}