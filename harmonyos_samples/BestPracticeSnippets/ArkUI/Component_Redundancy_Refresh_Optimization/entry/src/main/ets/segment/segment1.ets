// [Start Case1]
// constant declaration
const animationDuration: number = 500; // Move animation duration
const opacityChangeValue: number = 0.1; // The value of each change in opacity
const opacityChangeRange: number = 1; // Range of opacity changes
const translateYChangeValue: number = 180; // The value of translateY each time it changes
const translateYChangeRange: number = 250; // The range in which translateY changes
const scaleXChangeValue: number = 0.6; // The value of scaleX for each change
const scaleXChangeRange: number = 0.8; // The value of scaleX for each change


// Style Attribute Classes
class UIStyle {
  public translateX: number = 0;
  public translateY: number = 0;
  public scaleX: number = 0.3;
  public scaleY: number = 0.3;
}


@Component
struct ComponentA {
  @Link uiStyle: UIStyle; // Properties of uiStyle used by multiple components

  build() {
    Column() {
      // Components that use state variables
      SpecialImage({ specialImageUiStyle: this.uiStyle })
      Stack() {
        Column() {
          Image($r('app.media.startIcon'))
            .height(78)
            .width(78)
            .scale({
              x: this.uiStyle.scaleX,
              y: this.uiStyle.scaleY
            })
        }


        Stack() {
          Text('Hello World')
        }
      }
      .translate({
        x: this.uiStyle.translateX,
        y: this.uiStyle.translateY
      })


      // Modify the value of a state variable via a button click callback, causing the corresponding component to refresh.
      Column() {
        Button('Move')
          .onClick(() => {
            animateTo({ duration: animationDuration }, () => {
              this.uiStyle.translateY = (this.uiStyle.translateY + translateYChangeValue) % translateYChangeRange;
            })
          })
        Button('Scale')
          .onClick(() => {
            this.uiStyle.scaleX = (this.uiStyle.scaleX + scaleXChangeValue) % scaleXChangeRange;
          })
      }
    }
  }
}


@Component
struct SpecialImage {
  @Link specialImageUiStyle: UIStyle;
  private opacityNum: number = 0.5; // Default transparency

  private isRenderSpecialImage(): number {
    // Image transparency increases by 0.1 each time it is rendered, cycling between 0 and 1.
    this.opacityNum = (this.opacityNum + opacityChangeValue) % opacityChangeRange;
    return this.opacityNum;
  }

  build() {
    Column() {
      Image($r('app.media.startIcon'))
        .size({ width: 200, height: 200 })
        .scale({
          x: this.specialImageUiStyle.scaleX,
          y: this.specialImageUiStyle.scaleY
        })
        .opacity(this.isRenderSpecialImage())
      Text("SpecialImage")
    }
  }
}


@Entry
@Component
struct DFXStateBeforeOptimization {
  @State uiStyle: UIStyle = new UIStyle();

  build() {
    Column() {
      ComponentA({
        uiStyle: this.uiStyle
      })
    }
    .width('100%')
    .height('100%')
  }
}

// [End Case1]