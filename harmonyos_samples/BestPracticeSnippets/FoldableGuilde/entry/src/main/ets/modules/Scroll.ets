@Entry
@Component
export struct ScrollComponent {
  @State list: string[] = new Array(100).fill('')
  @State banner_height: number = 50
  scroll_start_timeout?: number
  scroll_stop_timeout?: number

  build() {
    Flex({ direction: FlexDirection.Column }) {
      Flex({ justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
        Text('top').fontSize(20)
      }.height(this.banner_height)

      List() {
        ForEach(this.list, () => {
          ListItem() {
            Text('北京').fontSize(24)
          }
        })
      }
      .onScrollStart(() => {
        if (this.scroll_stop_timeout) {
          clearTimeout(this.scroll_stop_timeout);
        }
        if (!this.scroll_start_timeout) {
          this.scroll_start_timeout = setTimeout(() => {
            this.banner_height = 0
          }, 2000)
        }
      })
      .onScrollStop(() => {
        if (this.scroll_start_timeout) {
          clearTimeout(this.scroll_start_timeout);
        }
        if (this.scroll_stop_timeout) {
          clearTimeout(this.scroll_stop_timeout);
        }
        this.scroll_stop_timeout = setTimeout(() => {
          this.banner_height = 50
        }, 2000)
      })
      .backgroundColor('#FFF1F3F5')
      .alignListItem(ListItemAlign.Center)

      Flex({ justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
        Text('bottom').fontSize(20)
      }.height(this.banner_height)
    }
  }
}