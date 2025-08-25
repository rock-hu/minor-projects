// [Start NavigationComponent]
@Component
export struct NavigationComponent {
  @State TooTmp: ToolbarItem = {'value': 'func', 'action': ()=> {}};
  private arr: number[] = [1, 2, 3];

  build() {
    Column() {
      // Root view container for routing navigation
      Navigation() {
        List({ space: 12 }) {
          ForEach(this.arr, (item: string) => {
            ListItem() {
              // Navigation component, which provides click response processing by default, does not require developers to customize click event logic。
              Navigation() {
                Text('NavRouter' + item)
                  .width('100%')
                  .height(72)
                  .backgroundColor('#FFFFFF')
                  .borderRadius(24)
                  .fontSize(16)
                  .fontWeight(500)
                  .textAlign(TextAlign.Center)
                // Non-home page display content
                NavDestination() {
                  Text('NavDestinationContent' + item)
                }
                .title('NavDestinationTitle' + item)
              }
            }
          }, (item: string): string => item)
        }
        .width('90%')
        .margin({ top: 12 })
      }
      .title('主标题')
      .mode(NavigationMode.Auto)
    }
    .height('100%')
    .width('100%')
    .backgroundColor('#F1F3F5')
  }
}

// [End NavigationComponent]