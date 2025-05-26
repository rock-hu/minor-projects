// [Start Counter_example5]
@Entry
@Component
struct Index {
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
  @Link currentIndex: number;
  private sizeFont: number = 50;


  isRender(): number {
    console.info(`ListItemComponent ${this.index} Text is rendered`);
    return this.sizeFont;
  }


  build() {
    Column() {
      Text(this.item)
        .fontSize(this.isRender())// Dynamically set the color of the text according to the difference between the index and currentIndex of the current list item.
        .fontColor(Math.abs(this.index - this.currentIndex) <= 1 ? Color.Red : Color.Blue)
        .onClick(() => {
          this.currentIndex = this.index;
        })
    }
  }
}
// [End Counter_example5]