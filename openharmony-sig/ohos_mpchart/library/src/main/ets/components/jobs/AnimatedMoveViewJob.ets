/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

import { Poolable } from '../utils/Poolable';
import { ObjectPool } from '../utils/ObjectPool';
import Transformer from '../utils/Transformer';
import ViewPortHandler from '../utils/ViewPortHandler';
import Chart from '../charts/ChartModel';
import ChartData from '../data/ChartData';
import IDataSet from '../interfaces/datasets/IDataSet';
import EntryOhos from '../data/EntryOhos';
import AnimatedViewPortJob from './AnimatedViewPortJob';
import { Animator as animator, AnimatorResult } from '@kit.ArkUI';

export default class AnimatedMoveViewJob extends AnimatedViewPortJob {
  private backAnimator: AnimatorResult | undefined = undefined

  public instantiate(): Poolable {
    return new AnimatedMoveViewJob(this.mViewPortHandler, this.xValue, this.yValue, this.mTrans, this.view, 0, 0, 0);
  }

  private static pool: ObjectPool<AnimatedMoveViewJob> = ObjectPool.create(4, new AnimatedMoveViewJob(null, 0, 0, null, null, 0, 0, 0))
    .setReplenishPercentage(0.5) as ObjectPool<AnimatedMoveViewJob>;

  public static getInstance(viewPortHandler: ViewPortHandler, xValue: number, yValue: number, trans: Transformer, v: Chart<ChartData<IDataSet<EntryOhos>>>,
    xOrigin: number, yOrigin: number, duration: number): AnimatedMoveViewJob {
    let result: AnimatedMoveViewJob = AnimatedMoveViewJob.pool.get();
    result.mViewPortHandler = viewPortHandler;
    result.xValue = xValue;
    result.yValue = yValue;
    result.mTrans = trans;
    result.view = v;
    result.xOrigin = xOrigin;
    result.yOrigin = yOrigin;
    result.duration = duration;
    return result;
  }

  public run(): void {
    this.backAnimator = animator.create({
      duration: this.duration,
      easing: "ease-in-out",
      delay: 0,
      fill: "forwards",
      direction: "normal",
      iterations: 1,
      begin: this.xOrigin,
      end: this.xValue
    })

    this.backAnimator.onFrame = (value: number) => {
      this.pts[0] = value;
      this.pts[1] = this.yOrigin + (this.yValue - this.yOrigin) * (value - this.xOrigin) / (this.xValue - this.xOrigin);
      if (this.mTrans) {
        this.mTrans.pointValuesToPixel(this.pts);
      }
      if (this.mViewPortHandler && this.view) {
        this.mViewPortHandler.centerViewPort(this.pts, this.view);
      }
    }
    this.backAnimator.onFinish = () => {
      AnimatedMoveViewJob.recycleInstance(this);
    }
    this.backAnimator.play();
  }

  public static recycleInstance(instance: AnimatedMoveViewJob): void {
    AnimatedMoveViewJob.pool.recycle(instance);
  }

  constructor(viewPortHandler: ViewPortHandler | null, xValue: number, yValue: number, trans: Transformer | null, v: Chart<ChartData<IDataSet<EntryOhos>>> | null,
    xOrigin: number, yOrigin: number, duration: number) {
    super(viewPortHandler, xValue, yValue, trans, v, xOrigin, yOrigin, duration);
  }
}
