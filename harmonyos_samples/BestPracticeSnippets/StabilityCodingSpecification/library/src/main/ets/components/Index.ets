/**
 * 最佳实践：NDK开发ArkTS侧编码规范
 */

// [Start NDKDevelopArkTSohPackage2]
import { hilog } from '@kit.PerformanceAnalysisKit';
// import rely on so
import testNapi from 'libentry.so';


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
            // call native
            hilog.info(0x0000, 'testTag', 'Test NAPI 2 + 3 = %{public}d', testNapi.add(2, 3));
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
// [End NDKDevelopArkTSohPackage2]