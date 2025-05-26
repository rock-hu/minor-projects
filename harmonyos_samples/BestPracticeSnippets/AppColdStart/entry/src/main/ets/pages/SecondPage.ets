// [Start not_recommend_demo]
// entry/src/main/ets/pages/SecondPage.ets
import { SubPage } from 'library/Index';
@Builder
export function SecondPageBuilder() {
  SecondPage()
}
@Entry
@Component
struct SecondPage {
  pathStack: NavPathStack = new NavPathStack();
  build() {
    NavDestination() {
      Row() {
        // Refer to the custom components of HAR
        SubPage()
      }
      .height('100%')
    }
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}
// [End not_recommend_demo]