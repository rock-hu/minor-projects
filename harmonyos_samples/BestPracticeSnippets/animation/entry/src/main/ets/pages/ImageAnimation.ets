// [Start image_animation]
// [Start change_animation]
@Component
export struct ImageAnimation {
  @State imageWidth: number = 60;
  @State imageHeight: number = 60;
  @State xPosition: number = 0;
  @State yPosition: number = 0;
  @State imageScaleX: number = 1;
  @State imageScaleY: number = 1;
  @State imageTranslateX: number = 0;
  @State imageTranslateY: number = 0;

  build() {
    Column() {
      Row() {
        Row() {
          Image($r('app.media.startIcon'))
            .width(this.imageWidth)
            .height(this.imageHeight)
            .position({ x: this.xPosition, y: this.yPosition })
            .scale({
              x: this.imageScaleX,
              y: this.imageScaleY,
              centerX: 0,
              centerY: 0
            })
            .translate({ x: this.imageTranslateX, y: this.imageTranslateY })
        }.width('33%')
        // [StartExclude image_animation]
        // [StartExclude change_animation]
        Row() {
          Image($r('app.media.startIcon'))
            .width(this.imageWidth)
            .height(this.imageHeight)
            .position({ x: this.xPosition, y: this.yPosition })
            .scale({
              x: this.imageScaleX,
              y: this.imageScaleY,
              centerX: 0,
              centerY: 0
            })
            .translate({ x: this.imageTranslateX, y: this.imageTranslateY })
        }.width('33%')

        Row() {
          Image($r('app.media.startIcon'))
            .width(this.imageWidth)
            .height(this.imageHeight)
            .position({ x: this.xPosition, y: this.yPosition })
            .scale({
              x: this.imageScaleX,
              y: this.imageScaleY,
              centerX: 0,
              centerY: 0
            })
            .translate({ x: this.imageTranslateX, y: this.imageTranslateY })
        }.width('33%')
        // [EndExclude image_animation]
        // [EndExclude change_animation]
      }
      .height('25%')
      // [StartExclude image_animation]
      // [StartExclude change_animation]
      Row() {
        Row() {
          Image($r('app.media.startIcon'))
            .width(this.imageWidth)
            .height(this.imageHeight)
            .position({ x: this.xPosition, y: this.yPosition })
            .scale({
              x: this.imageScaleX,
              y: this.imageScaleY,
              centerX: 0,
              centerY: 0
            })
            .translate({ x: this.imageTranslateX, y: this.imageTranslateY })
        }.width('33%')

        Row() {
          Image($r('app.media.startIcon'))
            .width(this.imageWidth)
            .height(this.imageHeight)
            .position({ x: this.xPosition, y: this.yPosition })
            .scale({
              x: this.imageScaleX,
              y: this.imageScaleY,
              centerX: 0,
              centerY: 0
            })
            .translate({ x: this.imageTranslateX, y: this.imageTranslateY })
        }.width('33%')

        Row() {
          Image($r('app.media.startIcon'))
            .width(this.imageWidth)
            .height(this.imageHeight)
            .position({ x: this.xPosition, y: this.yPosition })
            .scale({
              x: this.imageScaleX,
              y: this.imageScaleY,
              centerX: 0,
              centerY: 0
            })
            .translate({ x: this.imageTranslateX, y: this.imageTranslateY })
        }.width('33%')
      }
      .height('25%')

      Row() {
        Row() {
          Image($r('app.media.startIcon'))
            .width(this.imageWidth)
            .height(this.imageHeight)
            .position({ x: this.xPosition, y: this.yPosition })
            .scale({
              x: this.imageScaleX,
              y: this.imageScaleY,
              centerX: 0,
              centerY: 0
            })
            .translate({ x: this.imageTranslateX, y: this.imageTranslateY })
        }.width('33%')

        Row() {
          Image($r('app.media.startIcon'))
            .width(this.imageWidth)
            .height(this.imageHeight)
            .position({ x: this.xPosition, y: this.yPosition })
            .scale({
              x: this.imageScaleX,
              y: this.imageScaleY,
              centerX: 0,
              centerY: 0
            })
            .translate({ x: this.imageTranslateX, y: this.imageTranslateY })
        }.width('33%')

        Row() {
          Image($r('app.media.startIcon'))
            .width(this.imageWidth)
            .height(this.imageHeight)
            .position({ x: this.xPosition, y: this.yPosition })
            .scale({
              x: this.imageScaleX,
              y: this.imageScaleY,
              centerX: 0,
              centerY: 0
            })
            .translate({ x: this.imageTranslateX, y: this.imageTranslateY })
        }.width('33%')
        // [EndExclude change_animation]
        // [EndExclude image_animation]
      }
      .height('25%')

      Column() {
        // [StartExclude change_animation]
        Button('Change Layout Properties')
          .width('100%')
          // Key Attribute Settings
          .onClick(() => {
            // Loop animation by play count
            for (let i = 0; i < 10; i++) {
              // Interval playback position, width and height changes
              if (i % 2 == 0) {
                setTimeout(() => {
                  animateTo({ duration: 1000 }, () => {
                    this.imageWidth = 100;
                    this.imageHeight = 100;
                    this.xPosition = 12;
                    this.yPosition = 12;
                  })
                }, 1000 * i)
              } else {
                setTimeout(() => {
                  animateTo({ duration: 1000 }, () => {
                    this.imageWidth = 60;
                    this.imageHeight = 60;
                    this.xPosition = 0;
                    this.yPosition = 0;
                  })
                }, 1000 * i)
              }
            }
          })
        // [EndExclude change_animation]
        // [StartExclude image_animation]
        Button('Change Image Properties')
          .width('100%')
          .margin({ top: 12 })
          .onClick(() => {
            // Loop animation by play count
            for (let i = 0; i < 10; i++) {
              if (i % 2 == 0) {
                setTimeout(() => {
                  animateTo({ duration: 1000 }, () => {
                    this.imageScaleX = 2;
                    this.imageScaleY = 2;
                    this.imageTranslateX = 12;
                    this.imageTranslateY = 12;
                  })
                }, 1000 * i)
              } else {
                setTimeout(() => {
                  animateTo({ duration: 1000 }, () => {
                    this.imageScaleX = 1;
                    this.imageScaleY = 1;
                    this.imageTranslateX = 0;
                    this.imageTranslateY = 0;
                  })
                }, 1000 * i)
              }
            }
          })
        // [EndExclude image_animation]
      }
      .justifyContent(FlexAlign.End)
      .height('25%')
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
// [End image_animation]
// [End change_animation]