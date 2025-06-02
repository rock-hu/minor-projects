// [Start Case7]
@Entry
@Component
struct UseWatchListener {
  @State currentIndex: number = 0; // The subscript of the currently selected list item
  private listData: string[] = [];
  aboutToAppear(): void {
    for (let i = 0; i < 10; i++) {
      this.listData.push(`组件 ${i}`);
    }
  }
  build() {
    Row() {
      Column() {
        List() {
          ForEach(this.listData, (item: string, index: number) => {
            ListItem() {
              ListItemComponent({ item: item, index: index, currentIndex: this.currentIndex })
            }
          })
        }
        .height('100%')
        .width('100%')
        .alignListItem(ListItemAlign.Center)
      }
      .width('100%')
    }
    .height('100%')
  }
}
@Component
struct ListItemComponent {
  @Prop item: string;
  @Prop index: number; // The subscript of the list item
  @Link @Watch('onCurrentIndexUpdate') currentIndex: number;
  @State color: Color = Math.abs(this.index - this.currentIndex) <= 1 ? Color.Red : Color.Blue;
  isRender(): number {
    console.info(`ListItemComponent ${this.index} Text is rendered`);
    return 50;
  }
  onCurrentIndexUpdate() {
    // Dynamically modifies the value of color based on the difference between the index and currentIndex of the current list item.
    this.color = Math.abs(this.index - this.currentIndex) <= 1 ? Color.Red : Color.Blue;
  }
  build() {
    Column() {
      Text(this.item)
        .fontSize(this.isRender())
        .fontColor(this.color)
        .onClick(() => {
          this.currentIndex = this.index;
        })
    }
  }
}
// [End Case7]