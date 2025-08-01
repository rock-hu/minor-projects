@Entry
@Component
// [Start ScrollTest]
export struct ScrollTest {
  // Height of fixed area
  @State barHeight: number = 56;
  // Transparency of fixed area
  @State barOpacity: number = 1;
  // Current y-axis sliding offset
  @State currentYOffset: number = 0;

  build() {
    List() {
      // [StartExclude ScrollTest]
      ForEach(new Array(100).fill(''), () => {
        ListItem() {
          Text('北京').fontSize(24)
        }
      })
      // [EndExclude ScrollTest]
    }
    .onScrollFrameBegin((offset: number, state: ScrollState) => {
      this.currentYOffset += Math.abs(offset);
      // Take the Y-axis offset of 100 as an example. When the offset is less than 100, the fixed area is gradually hidden
      if (this.currentYOffset <= 100) {
        this.barHeight = 56 * (1 - this.currentYOffset / 100);
        this.barOpacity = 1 - this.currentYOffset / 100;
      }
      // Directly hide the fixed area when the offset is greater than 100
      else {
        this.barHeight = 0;
        this.barOpacity = 0;
      }
      return { offsetRemain: offset };
    })
    .onScrollStart(() => {
      // Reset the offset of Y-axis sliding at the beginning of sliding
      this.currentYOffset = 0;
    })
    .onScrollStop(() => {
      // Use animation to restore the height and transparency of the fixed area when sliding stops
      setTimeout(() => {
        this.getUIContext().animateTo({
          duration: 300
        }, () => {
          this.barHeight = 56;
          this.barOpacity = 1;
        })
      }, 2000);
    })
  }
}
// [End ScrollTest]