import web_webview from '@ohos.web.webview';

@Entry
@Component
export struct NestedScroll {
  private scrollerForScroll: Scroller = new Scroller()
  controller: web_webview.WebviewController = new web_webview.WebviewController();
  // scroll竖向的滚动
  @State ScrollDirection: ScrollDirection = ScrollDirection.Vertical
  title = '2023落下帷幕，一起迎接2024年*****************************************'
  review = '评论'
  context = '一起迎接2024年，************************，************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************'
  list = [0, 0, 0, 0]

  build() {
    Flex() {
      Scroll(this.scrollerForScroll) {
        Column({ space: 5 }) {
          Text(this.title).fontSize(40).padding(40)
          List() {
            ForEach(this.list, () => {
              ListItem() {
                Text(this.context).fontSize(16)
              }
            }, (item: string) => item)
          }

          Web({ src: $rawfile("scroll.html"), controller: this.controller })
            .nestedScroll({
              scrollForward: NestedScrollMode.SELF_FIRST,
              scrollBackward: NestedScrollMode.SELF_FIRST
            })
            .height("100%")
            .width("100%")
            .layoutMode(WebLayoutMode.FIT_CONTENT)

          Text(this.review).fontSize(30)
          List() {
            ForEach(this.list, () => {
              ListItem() {
                Text(this.context).fontSize(16)
              }
            }, (item: string) => item)
          }
        }.width("95%")
      }
      .scrollBar(BarState.Off)
      .width("100%")
      .height("120%")
      .scrollable(this.ScrollDirection)
    }.width('100%').height('100%')
  }
}

// // xxx.ets
// import web_webview from '@ohos.web.webview';
//
// @Entry
// @Component
// export struct NestedScroll {
//   private scrollerForScroll: Scroller = new Scroller()
//   controller: web_webview.WebviewController = new web_webview.WebviewController();
//   controller2: web_webview.WebviewController = new web_webview.WebviewController();
//   // NestedScrollMode设置成SELF_ONLY时，Web网页滚动到页面边缘后，不与父组件联动，父组件仍无法滚动。
//   @State NestedScrollMode0: NestedScrollMode = NestedScrollMode.SELF_ONLY
//   // NestedScrollMode设置成SELF_FIRST时，Web网页滚动到页面边缘后，父组件继续滚动。
//   @State NestedScrollMode1: NestedScrollMode = NestedScrollMode.SELF_FIRST
//   // NestedScrollMode设置为PARENT_FIRST时，父组件先滚动，滚动至边缘后通知Web继续滚动。
//   @State NestedScrollMode2: NestedScrollMode = NestedScrollMode.PARENT_FIRST
//   // NestedScrollMode设置为PARALLEL时，父组件与Web同时滚动。
//   @State NestedScrollMode3: NestedScrollMode = NestedScrollMode.PARALLEL
//   @State NestedScrollModeF: NestedScrollMode = NestedScrollMode.SELF_FIRST
//   @State NestedScrollModeB: NestedScrollMode = NestedScrollMode.SELF_FIRST
//   // scroll竖向的滚动
//   @State ScrollDirection: ScrollDirection = ScrollDirection.Vertical
//   title = '2023落下帷幕，一起迎接2024年*****************************************'
//   review = '评论'
//   context = '一起迎接2024年，************************，************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************'
//   list = [0, 0, 0, 0]
//
//   build() {
//     Flex() {
//       Scroll(this.scrollerForScroll) {
//
//
//         Column({ space: 5 }) {
//           Text(this.title).fontSize(40).padding(40)
//           List() {
//             ForEach(this.list, () => {
//               ListItem() {
//                 Text(this.context).fontSize(16)
//               }
//             })
//           }
//
//           Web({ src: $rawfile("scroll.html"), controller: this.controller })
//             .nestedScroll({
//               scrollForward: NestedScrollMode.SELF_FIRST,
//               scrollBackward: NestedScrollMode.SELF_FIRST
//             })
//             .height("100%")
//             .width("100%")
//             // .scrollBar(BarState.Off)
//
//
//           Text(this.review).fontSize(30).padding(20)
//
//           List() {
//             ForEach(this.list, () => {
//               ListItem() {
//                 Text(this.context).fontSize(16)
//               }
//             })
//           }
//
//         }.width("95%").border({ width: 5 })
//       }
//       .scrollBar(BarState.Off)
//       .width("100%")
//       .height("120%")
//       .border({ width: 5 })
//       .scrollable(this.ScrollDirection)
//     }.width('100%').height('100%').backgroundColor(0xDCDCDC).padding(20)
//   }
// }
