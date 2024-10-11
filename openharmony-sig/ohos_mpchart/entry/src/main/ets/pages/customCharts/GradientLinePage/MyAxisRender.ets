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

import { Utils, YAxisRenderer, AxisDependency } from '@ohos/mpchart';

export default class MyAxisRender extends YAxisRenderer {
  public renderAxisLine(c: CanvasRenderingContext2D, extraLength: number): void {

    if (!this.mYAxis || !this.mViewPortHandler || !this.mYAxis.isEnabled() || !this.mYAxis.isDrawAxisLineEnabled()) {
      return;
    }

    if (this.mAxisLinePaint) {
      this.mAxisLinePaint.setColor(this.mYAxis.getAxisLineColor());
      this.mAxisLinePaint.setStrokeWidth(this.mYAxis.getAxisLineWidth());

      Utils.resetContext2DWithoutFont(c, this.mAxisLinePaint);
      if (this.mYAxis.getAxisDependency() == AxisDependency.LEFT) {
        c.beginPath()
        let grad = c.createLinearGradient(0, this.mViewPortHandler.contentBottom() + extraLength, 0,
          this.mViewPortHandler.contentTop());
        grad.addColorStop(0.0, '#00ff00')
        grad.addColorStop(1.0, '#ff0000')
        c.strokeStyle = grad;
        c.moveTo(this.mViewPortHandler.contentLeft(), this.mViewPortHandler.contentTop());
        c.lineTo(this.mViewPortHandler.contentLeft(), this.mViewPortHandler.contentBottom() + extraLength)
        c.stroke();
        c.closePath();
      } else {
        c.beginPath()
        c.moveTo(this.mViewPortHandler.contentRight(), this.mViewPortHandler.contentTop());
        c.lineTo(this.mViewPortHandler.contentRight(), this.mViewPortHandler.contentBottom() + extraLength)
        c.stroke();
        c.closePath();
      }
    }
  }
}