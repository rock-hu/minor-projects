// [Start segment3_start]
@Component
struct ArticleCardView {
  build() {
    Column() {
      Column() {
        Column() {
          // ...                  // Deliberate nesting with a maximum nesting level of 25 layers
        }.width("100%")
      }.width("100%")
    }.width("100%")
  }
}
// [End segment3_start]