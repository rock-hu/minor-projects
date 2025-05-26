// [Start segment1_start]
@Component
export struct ArticleCardView {
  build() {
    Row() {                  // Linear layout, layer 1
      Column() {             // Linear layout, layer 2
        Column() {
          Text()
          Text()
        }
        Row() {              // Linear layout, layer 3
          Row(){             // Linear layout, layer 4
            Image('')          // Linear layout, layer 5
            Text()
          }
          Row(){
            Image('')
            Text()
          }
          Row(){
            Image('')
            Text()
          }
        }
      }
      Image('')
    }
  }
}
// [End segment1_start]