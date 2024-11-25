import { LineChartRenderer } from '@ohos/mpchart';

export default class AlphaDataRender extends LineChartRenderer {
  public drawExtras(c: CanvasRenderingContext2D) {
    c.globalAlpha = 0.1;
  }
}