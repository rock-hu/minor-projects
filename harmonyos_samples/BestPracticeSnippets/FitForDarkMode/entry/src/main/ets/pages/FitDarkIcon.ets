// [Start media_start]
@Component
struct Home {
  // ...

  build() {
    Scroll() {
      Column() {
        // ...

        Stack({ alignContent: Alignment.TopStart }) {
          Image($r('app.media.bell'))
            .width('100%')
            .borderRadius(12)
            .objectFit(ImageFit.Cover)

          // ...
        }
        // ...
      }
    }
    // ...
  }
}
// [End media_start]