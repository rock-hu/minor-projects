// [Start Case2]
// constant declaration
const animationDuration: number = 500; // Move animation duration
const opacityChangeValue: number = 0.1; // The value of each change in opacity
const opacityChangeRange: number = 1; // Range of opacity changes
const translateYChangeValue: number = 180; // The value of translateY each time it changes
const translateYChangeRange: number = 250; // The range in which translateY changes
const scaleXChangeValue: number = 0.6; // The value of scaleX for each change
const scaleXChangeRange: number = 0.8; // The value of scaleX for each change

// Style property class, nested ScaleStyle, TranslateStyle
@Observed
class UIStyle {
  translateStyle: TranslateStyle = new TranslateStyle();
  scaleStyle: ScaleStyle = new ScaleStyle();
}

// Zoom Property Class
@Observed
class ScaleStyle {
  public scaleX: number = 0.3;
  public scaleY: number = 0.3;
}

// Displacement Attribute Class
@Observed
class TranslateStyle {
  public translateX: number = 0;
  public translateY: number = 0;
}

@Component
struct ComponentA {
  @ObjectLink scaleStyle: ScaleStyle;
  @ObjectLink translateStyle: TranslateStyle;

  build() {
    Column() {
      SpecialImage({
        specialImageScaleStyle: this.scaleStyle
      })
      // Other UI components
      Stack() {
        Column() {
          Image($r('app.media.startIcon'))
            .scale({
              x: this.scaleStyle.scaleX,
              y: this.scaleStyle.scaleY
            })
        }

        Stack() {
          Text('Hello World')
        }
      }
      .translate({
        x: this.translateStyle.translateX,
        y: this.translateStyle.translateY
      })

      // Modify the value of a state variable via a button click callback, causing the corresponding component to refresh.
      Column() {
        Button('Move')
          .onClick(() => {
            this.getUIContext().animateTo({ duration: animationDuration }, () => {
              this.translateStyle.translateY =
                (this.translateStyle.translateY + translateYChangeValue) % translateYChangeRange;
            })
          })
        Button('Scale')
          .onClick(() => {
            this.scaleStyle.scaleX = (this.scaleStyle.scaleX + scaleXChangeValue) % scaleXChangeRange;
          })
      }
    }
  }
}

@Component
struct SpecialImage {
  @Link specialImageScaleStyle: ScaleStyle;
  private opacityNum: number = 0.5; // Default transparency

  // isRenderSpecialImage function
  private isRenderSpecialImage(): number {
    // Image transparency increases by 0.1 each time it is rendered, cycling between 0 and 1.
    this.opacityNum = (this.opacityNum + opacityChangeValue) % opacityChangeRange;
    return this.opacityNum;
  }

  build() {
    Column() {
      Image($r('app.media.startIcon'))
        .scale({
          x: this.specialImageScaleStyle.scaleX,
          y: this.specialImageScaleStyle.scaleY
        })
        .opacity(this.isRenderSpecialImage())
      Text("SpecialImage")
    }
  }
}

@Entry
@Component
struct DFXStateAfterOptimization {
  @State uiStyle: UIStyle = new UIStyle();

  build() {
    Stack() {
      ComponentA({
        scaleStyle: this.uiStyle.scaleStyle,
        translateStyle: this.uiStyle.translateStyle,
      })
    }
  }
}

// [End Case2]