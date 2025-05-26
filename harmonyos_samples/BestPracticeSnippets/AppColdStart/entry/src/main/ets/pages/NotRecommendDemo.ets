// [Start not_recommend_demo]
// entry/src/main/ets/pages/Index.ets
import { MainPage } from 'library/Index'; // Unrecommended usage: Direct import of subPage.ets files related to cold start non-strong
@Component
export struct Index{
  @Provide pathStack: NavPathStack = new NavPathStack();
  build() {
    Navigation(this.pathStack) {
      Row() {
        // Refer to the custom components of HAR
        MainPage()
      }
    }
  }
}
// [End not_recommend_demo]