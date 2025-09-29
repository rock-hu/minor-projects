// [Start PinchImage]
@Component
export struct PinchImage {
  list: string[] = ['image1', 'image2', 'image3', 'image4', 'image5', 'image6'];
  @State GridColumn: string = '1fr 1fr 1fr';
  @State GridRow: string = '1fr 1fr';

  build() {
    Column() {
      Grid() {
        ForEach(this.list, (item: string) => {
          GridItem() {
            Text(item)
          }.backgroundColor(Color.Grey)
        }, (item: string) => item)
      }
      .columnsTemplate(this.GridColumn)
      .rowsTemplate(this.GridRow)
      .rowsGap(12)
      .columnsGap(12)
    }
    // Change the layout of the Grid when two-finger scaling is triggered
    .gesture(PinchGesture({ fingers: 2 }).onActionUpdate((event: GestureEvent) => {
      if (event.scale > 1) {
        // Increase animation effect
        this.getUIContext().animateTo({
          duration: 500
        }, () => {
          // When the two-finger scaling ratio is > 1, the number of Grid columns becomes 2
          this.GridColumn = '1fr 1fr';
        })
      } else {
        this.getUIContext().animateTo({
          duration: 500
        }, () => {
          // When the two-finger scaling ratio is < =1, the number of Grid columns becomes 3
          this.GridColumn = '1fr 1fr 1fr';
        })
      }
    }))
  }
}

// [End PinchImage]