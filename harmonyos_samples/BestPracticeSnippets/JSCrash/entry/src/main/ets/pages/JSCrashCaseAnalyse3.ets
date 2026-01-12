/**
 * 最佳实践：JSCrash问题案例分析
 */

import { hidebug } from '@kit.PerformanceAnalysisKit';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    RelativeContainer() {
      Text(this.message)
        .id('HelloWorld')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .onClick(()=>{
          hidebug.getPss();
          // [Start throw_new_error_case]
          // In a production environment, encrypted error codes should be used instead of plain text descriptions.
          throw new Error("TEST JS ERROR")
          // [End throw_new_error_case]
        })

    }
    .height('100%')
    .width('100%')
  }
}