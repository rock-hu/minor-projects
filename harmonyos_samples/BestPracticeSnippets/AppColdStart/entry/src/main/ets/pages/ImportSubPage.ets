// [Start import_sub_page]
// SecondPage.ets
import { SubPage } from 'library/IndexOthers';

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
// [End import_sub_page]