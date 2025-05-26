// [Start import_main_page]
// Index.ets
import { MainPage } from 'library/IndexAppStart';

@Entry
@Component
struct Index {
  @Provide pathStack: NavPathStack = new NavPathStack();

  build() {
    Navigation(this.pathStack) {
      Row() {
        // Refer to the custom components of HAR
        MainPage()
      }
    }
    .height('100%')
    .width('100%')
  }
}
// [End import_main_page]