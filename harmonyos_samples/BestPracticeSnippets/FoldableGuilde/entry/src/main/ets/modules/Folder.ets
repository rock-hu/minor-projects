// [Start Folder]
@Entry
@Component
export struct Folder {
  @State len_wid: number = 480
  @State w: string = "40%"
  build() {
    Column() {
      // UpperItems puts the required id hovering to the upper half of the screen into upperItems, and the other components will be stacked in the lower half of the screen
      FolderStack({ upperItems: ["upperitemsId"] }) {
        // This Column will automatically move up to the upper half of the screen
        Column() {
          Text("video zone").height("100%").width("100%").textAlign(TextAlign.Center).fontSize(25)
        }.backgroundColor(Color.Pink).width("100%").height("100%").id("upperitemsId")

        // The following two Column are stacked in the lower half screen area
        Column() {
          Text("video title")
            .width("100%")
            .height(50)
            .textAlign(TextAlign.Center)
            .backgroundColor(Color.Red)
            .fontSize(25)
        }.width("100%").height("100%").justifyContent(FlexAlign.Start)

        Column() {
          Text("video bar")
            .width("100%")
            .height(50)
            .textAlign(TextAlign.Center)
            .backgroundColor(Color.Red)
            .fontSize(25)
        }.width("100%").height("100%").justifyContent(FlexAlign.End)
      }
      .backgroundColor(Color.Yellow)
      // Whether to start the dynamic effect
      .enableAnimation(true)
      // Whether to rotate automatically
      .autoHalfFold(true)
      // FolderStack callback callback when the folding state changes
      .onFolderStateChange((msg) => {
        if (msg.foldStatus === FoldStatus.FOLD_STATUS_EXPANDED) {
          console.info("The device is currently in the expanded state")
        } else if (msg.foldStatus === FoldStatus.FOLD_STATUS_HALF_FOLDED) {
          console.info("The device is currently in the half folded state")
        } else {
          // .............
        }
      })
      // If the folderStack does not fill the full screen of the page, it can be used as an ordinary Stack
      .alignContent(Alignment.Bottom)
      .height("100%")
      .width("100%")
      .borderWidth(1)
      .backgroundColor(Color.Yellow)

    }
    .height("100%")
    .width("100%")
    .borderWidth(1)
    .backgroundColor(Color.Pink)
    .expandSafeArea([SafeAreaType.SYSTEM], [SafeAreaEdge.BOTTOM])
  }
}
// [End Folder]