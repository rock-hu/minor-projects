// [Start style_example]
// xxx.ets
@Entry
@Component
struct StyleExample {
  @Styles
  pressedStyles(): void {
    .backgroundColor("#ED6F21")
  }
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
      Button("pressed")
        .width(100)
        .height(25)
        .stateEffect(false)
        .stateStyles({
          pressed: this.pressedStyles,
        })
    }
    .width(350).height(300)
  }
}
// [End style_example]