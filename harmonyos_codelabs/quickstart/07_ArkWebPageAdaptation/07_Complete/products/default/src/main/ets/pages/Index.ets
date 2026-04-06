// import { QuickStartPage } from '@ohos/quickstart/';
import { CourseLearning } from 'learning';


@Entry
@Component
struct Index {
  build() {
    Column() {
      // QuickStartPage()
      CourseLearning()
    }
    .backgroundColor('#F1F3F5')
    .padding({ top: 36, bottom: 28 })
  }
}