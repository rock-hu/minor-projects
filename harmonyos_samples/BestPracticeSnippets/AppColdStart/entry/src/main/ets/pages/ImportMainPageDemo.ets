// [Start import_main_page_demo]
// Index.ets
import { MainPage } from 'library/src/main/ets/components/mainpage/MainPage';

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
// [End import_main_page_demo]