@Entry
@Component
struct DynamicIcons_startDynamicIcon {
  @Builder
  MyMenu(){
    Menu() {
      MenuItem({ startIcon: $r('app.media.app_icon') })
      MenuItem({ startIcon: $r('app.media.app_icon') })
      MenuItem({ startIcon: $r('app.media.app_icon') })
      MenuItem({ startIcon: $r('app.media.app_icon') })
      MenuItem({ startIcon: $r('app.media.app_icon') })
    }
  }
  build() {
    Row() {
      Text('点击展示菜单')
        .fontSize(30)
        .fontWeight(FontWeight.Bold)
      Text('查看动态图标是否显示')
        .fontSize(14)
        .fontColor('blue')
    }
    .bindMenu(this.MyMenu)
  }
}