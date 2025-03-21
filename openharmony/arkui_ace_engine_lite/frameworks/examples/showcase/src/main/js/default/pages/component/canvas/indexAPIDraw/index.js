/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

import { backPage, routePage } from '../../../../common/js/general';
export default {
  ...backPage('pages/component/canvas/indexAPI/index'),
  fillRectFunc: function() {
    let canvasRef = this.$refs.canvas1;
    let ctx = canvasRef.getContext('2d');
    ctx.fillRect(20, 20, 100, 150);
  },
  strokeRectFunc: function() {
    let canvasRef = this.$refs.canvas2;
    let ctx = canvasRef.getContext('2d');
    ctx.strokeRect(20, 20, 100, 150);
  },
  fillTextFunc: function() {
    let canvasRef = this.$refs.canvas3;
    let ctx = canvasRef.getContext('2d');
    ctx.font = '30px SourceHanSansSC-Regular';
    ctx.fillText('Hello JS', 10, 50);
  },
  arcFunc: function() {
    let canvasRef = this.$refs.canvas4;
    let ctx = canvasRef.getContext('2d');
    ctx.beginPath();
    ctx.arc(50, 50, 50, Math.PI * 2 * (1 / 36), Math.PI * 2 * (5 / 36), true);
    ctx.stroke();
  }
};
