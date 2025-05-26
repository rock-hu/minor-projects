// [Start index_view]
import GlassCoverView from './GlassCoverView';

@Entry
@Component
struct Index {
  @State pointsToDraw: number[][] = [];

  /**
   * Make a list of 1000 0-1 positions and draw circles at the corresponding positions
   */
  startDraw(): void {
    this.pointsToDraw = [];
    for (let index = 0; index < 1000; index++) {
      this.pointsToDraw.push([Math.random(), Math.random()]);
    }
  }

  build() {
    Stack() {
      Image($r('app.media.layered_image'))
        .width('100%')
        .height('100%')
      // Transparent circle custom component, in which 1000 transparent circles are drawn
      GlassCoverView({ pointsToDraw: this.pointsToDraw })
        .width('100%')
        .height('100%')
      Row() {
        Button('Begin Draw')
          .width('100%')
          .height(40)
      }
      .padding({
        right: 16,
        bottom: 16,
        left: 16
      })
      .onClick(() => {
        this.startDraw();
      })
    }
    .alignContent(Alignment.Bottom)
    .width('100%')
    .height('100%')
  }
}
// [End index_view]