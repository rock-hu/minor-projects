// [Start glass_cover_view]
import { hiTraceMeter } from '@kit.PerformanceAnalysisKit';

/**
 * Glass cladding effect
 */
@Preview
@Component
export default struct GlassCoverView {
  @Prop @Watch('onDraw') pointsToDraw: number[][] = [];
  private settings = new RenderingContextSettings(true);
  private renderContext = new CanvasRenderingContext2D(this.settings);
  private viewWidth: number = 0;
  private viewHeight: number = 0;

  build() {
    Stack() {
      Canvas(this.renderContext)
        .width('100%')
        .height('100%')
        .onAreaChange((_: Area, newValue: Area) => {
          this.handleAreaChange(newValue);
        })
    }
    .height('100%')
    .width('100%')
  }

  private handleAreaChange(area: Area): void {
    this.viewWidth = parseInt(area.width.toString());
    this.viewHeight = parseInt(area.height.toString());
    this.onDraw();
  }

  private onDraw(): void {
    const canvas = this.renderContext;
    canvas.reset();
    if (canvas === undefined) {
      return;
    }
    // Hollow transparent circle
    hiTraceMeter.startTrace('slow', 1);
    console.info('debug: slow start');
    // Save drawing context
    canvas.save();
    // Clears the specified pixel within the given rectangle
    canvas.clearRect(0, 0, this.viewWidth, this.viewHeight);
    // Specifies the fill color of the drawing
    canvas.fillStyle = '#77CCCCCC';
    // Fill a rectangle
    canvas.fillRect(0, 0, this.viewWidth, this.viewHeight);
    // Draw a hollow circle
    canvas.globalCompositeOperation = 'destination-out';
    canvas.fillStyle = '#CCCCCC';
    this.pointsToDraw.forEach((xy: number[]) => {
      this.drawOneCell(canvas, xy[0] * this.viewWidth, xy[1] * this.viewHeight, px2vp(15));
    })
    canvas.fill();
    // Restore the saved drawing context
    canvas.restore();
    console.info('debug: slow end');
    hiTraceMeter.finishTrace('slow', 1);
  }

  /**
   * Draw a circle according to the specified position and width
   */
  private drawOneCell(canvas: CanvasRenderer, x: number, y: number, width: number): void {
    canvas.moveTo(x + width, y);
    canvas.arc(x, y, width, 0, Math.PI * 2);
  }
}
// [End glass_cover_view]