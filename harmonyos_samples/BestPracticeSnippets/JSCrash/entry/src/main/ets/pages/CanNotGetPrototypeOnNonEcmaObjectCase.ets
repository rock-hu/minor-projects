/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start can_not_get_prototype_on_non_ecma_object_case]
import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

declare class ArkTools {
  static hintGC(): void;
}

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
            hilog.info(0x0000, 'testTag', 'Test NAPI 2 + 3 = %{public}d', testNapi.add(2, 3));
            ArkTools.hintGC(); // 触发GC
            // setTimeout(()=> {  // 异步方法
            testNapi.getValue().length;
            // }, 1000);
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}

// [End can_not_get_prototype_on_non_ecma_object_case]