// [Start explicit_animate_to]
@Component
export struct ExplicitAnimateTo {
  @State widthSize: number = 80;
  @State heightSize: number = 40;
  @State flag: boolean = true;

  build() {
    Column() {
      Column() {
        Row() {
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          // [StartExclude explicit_animate_to]
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          // [EndExclude explicit_animate_to]
        }

        // [StartExclude explicit_animate_to]
        Row() {
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
        }

        Row() {
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
        }

        Row() {
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
        }

        Row() {
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
        }

        Row() {
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
        }

        Row() {
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
          Button()
            .width(this.widthSize)
            .height(this.heightSize)
            .margin(20)
        }

        // [EndExclude explicit_animate_to]
      }
      .height('85%')

      Column() {
        Button('Tap me')
          .width('100%')
          .onClick((event?: ClickEvent | undefined) => {
            // Animate the width and height properties of the Button component
            if (this.flag) {
              this.getUIContext().animateTo({
                duration: 2000,
                curve: Curve.Linear,
                delay: 500,
                iterations: 1,
                playMode: PlayMode.Normal
              }, () => {
                this.widthSize = 50;
                this.heightSize = 25;
              })
            } else {
              this.getUIContext().animateTo({
                duration: 2000,
                curve: Curve.Linear,
                delay: 500,
                iterations: 1,
                playMode: PlayMode.Normal
              }, () => {
                this.widthSize = 80;
                this.heightSize = 40;
              })
            }
            this.flag = !this.flag;
          })
      }
      .height('15%')
    }
    .padding({
      left: 16,
      right: 16,
      bottom: 16
    })
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Start)
  }
}
// [End explicit_animate_to]