import { webview } from '@kit.ArkWeb';

@Component
export struct NestedScroll {
  private scrollerForScroll: Scroller = new Scroller()
  controller: webview.WebviewController = new webview.WebviewController();
  // scroll竖向的滚动
  @State ScrollDirection: ScrollDirection = ScrollDirection.Vertical;
  title = '2023落下帷幕，一起迎接2024年*****************************************';
  review = '评论';
  context = '一起迎接2024年，************************，************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************';
  list = [0, 0, 0, 0];

  build() {
    Flex() {
      Scroll(this.scrollerForScroll) {
        Column({ space: 5 }) {
          Text(this.title)
            .fontSize(40)
            .padding(40)
          List() {
            ForEach(this.list, () => {
              ListItem() {
                Text(this.context).fontSize(16)
              }
            }, (item: string) => item)
          }

          Web({ src: $rawfile('scroll.html'), controller: this.controller })
            .nestedScroll({
              scrollForward: NestedScrollMode.SELF_FIRST,
              scrollBackward: NestedScrollMode.SELF_FIRST
            })
            .height('100%')
            .width('100%')
            .layoutMode(WebLayoutMode.FIT_CONTENT)

          Text(this.review).fontSize(30)
          List() {
            ForEach(this.list, () => {
              ListItem() {
                Text(this.context)
                  .fontSize(16)
              }
            }, (item: string) => item)
          }
        }.width('95%')
      }
      .scrollBar(BarState.Off)
      .width('100%')
      .height('120%')
      .scrollable(this.ScrollDirection)
    }
    .width('100%')
    .height('100%')
  }
}