/**
 * 最佳实践：NDK开发ArkTS侧编码规范
 */

// [Start NDKDevelopArkTSohPackage]
import { hilog } from '@kit.PerformanceAnalysisKit';
// Reference napi in dependent modules
import { testNapi } from 'library';


@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            // Call methods in Napi
            hilog.info(0x0000, 'testTag', 'Test NAPI 2 + 3 = %{public}d', testNapi.add(1, 2));
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
// [End NDKDevelopArkTSohPackage]