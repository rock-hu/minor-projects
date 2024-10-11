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

import { EntryOhos, ILineDataSet, Style, Transformer, Utils, LineChartRenderer } from '@ohos/mpchart';
import { MyLineDataSet } from './MyLineDataSet';

export default class MyRender extends LineChartRenderer{
  protected drawCubicBezier(c: CanvasRenderingContext2D, dataSet: MyLineDataSet) {
    if(dataSet.interval == undefined){
      super.drawCubicBezier(c, dataSet);
      return;
    }
    if (!this.mChart || !this.mXBounds) {
      return;
    }
    const phaseY: number = this.mAnimator ? this.mAnimator.getPhaseY() : 1;
    const trans: Transformer | null = this.mChart.getTransformer(dataSet.getAxisDependency());

    this.mXBounds.set(this.mChart, dataSet);

    const intensity: number = dataSet.getCubicIntensity();

    let cubicPath = new Path2D();
    //实线
    let solidLinePath = new Path2D();
    //虚线
    let dashedLinePath = new Path2D();
    if (this.mXBounds.range >= 1) {
      let prevDx: number = 0;
      let prevDy: number = 0;
      let curDx: number = 0;
      let curDy: number = 0;

      // Take an extra point from the left, and an extra from the right.
      // That's because we need 4 points for a cubic bezier (cubic=4), otherwise we get lines moving and doing weird stuff on the edges of the chart.
      // So in the starting `prev` and `cur`, go -2, -1
      // And in the `lastIndex`, add +1

      const firstIndex: number = this.mXBounds.min + 1;
      const lastIndex: number = this.mXBounds.min + this.mXBounds.range;

      let prevPrev: EntryOhos | null;
      let prev: EntryOhos | null = dataSet.getEntryForIndex(Math.max(firstIndex - 2, 0));
      let cur: EntryOhos | null = dataSet.getEntryForIndex(Math.max(firstIndex - 1, 0));
      let next: EntryOhos | null = cur;
      let nextIndex: number = -1;

      if (cur === null) return;

      Utils.resetContext2DWithoutFont(c, this.mRenderPaint);
      // let the spline start
      cubicPath.moveTo(cur.getX(), cur.getY() * phaseY);
      solidLinePath.moveTo(cur.getX(), cur.getY() * phaseY);

      for (let j: number = this.mXBounds.min + 1; j <= this.mXBounds.range + this.mXBounds.min; j++) {
        prevPrev = prev;
        prev = cur;
        cur = nextIndex === j ? next : dataSet.getEntryForIndex(j);

        nextIndex = j + 1 < dataSet.getEntryCount() ? j + 1 : j;
        next = dataSet.getEntryForIndex(nextIndex);

        prevDx = (cur.getX() - prevPrev.getX()) * intensity;
        prevDy = (cur.getY() - prevPrev.getY()) * intensity;
        curDx = (next.getX() - prev.getX()) * intensity;
        curDy = (next.getY() - prev.getY()) * intensity;

        cubicPath.bezierCurveTo(
          prev.getX() + prevDx,
          (prev.getY() + prevDy) * phaseY,
          cur.getX() - curDx,
          (cur.getY() - curDy) * phaseY,
          cur.getX(),
          cur.getY() * phaseY
        );
        if(j <= dataSet.interval){
          solidLinePath.bezierCurveTo(
            prev.getX() + prevDx,
            (prev.getY() + prevDy) * phaseY,
            cur.getX() - curDx,
            (cur.getY() - curDy) * phaseY,
            cur.getX(),
            cur.getY() * phaseY
          );
          if(j == dataSet.interval) {
            dashedLinePath.moveTo(cur.getX(),
              cur.getY() * phaseY);
          }
        }else{
          dashedLinePath.bezierCurveTo(
            prev.getX() + prevDx,
            (prev.getY() + prevDy) * phaseY,
            cur.getX() - curDx,
            (cur.getY() - curDy) * phaseY,
            cur.getX(),
            cur.getY() * phaseY
          );
        }
      }
    }

    // if filled is enabled, close the path
    if (dataSet.isDrawFilledEnabled()) {
      let cubicFillPath: Path2D = new Path2D();
      // cubicFillPath.reset();
      cubicFillPath.addPath(cubicPath);

      if (c && trans) {
        this.drawCubicFill(c, dataSet, cubicFillPath, trans, this.mXBounds);
      }
    }

    this.mRenderPaint.setColor(dataSet.getColor());
    this.mRenderPaint.setStyle(Style.STROKE);

    if (trans && trans.pathValueToPixel(cubicPath)) {
      cubicPath = trans.pathValueToPixel(cubicPath);
      solidLinePath = trans.pathValueToPixel(solidLinePath);
      dashedLinePath = trans.pathValueToPixel(dashedLinePath);
    }

    Utils.resetContext2DWithoutFont(c, this.mRenderPaint);
    c.beginPath();
    c.stroke(solidLinePath);
    c.closePath();

    Utils.resetContext2DWithoutFont(c, this.mRenderPaint);
    c.beginPath();
    c.setLineDash([5,5,0]);
    c.stroke(dashedLinePath);
    c.closePath();
    this.mRenderPaint.setDashPathEffect(null);
  }

}