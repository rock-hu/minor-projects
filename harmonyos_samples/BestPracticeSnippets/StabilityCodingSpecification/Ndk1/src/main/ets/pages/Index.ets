import { hilog } from '@kit.PerformanceAnalysisKit';

// [Start testNapi_ets]
import testNapi from 'liblibrary.so';
export {testNapi}
// [End testNapi_ets]

const DOMAIN = 0x0000;


@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize($r('app.float.page_text_font_size'))
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = 'Welcome';
            hilog.info(DOMAIN, 'testTag', 'Test NAPI 2 + 3 = %{public}d', testNapi.add(2, 3));
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
