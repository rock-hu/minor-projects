// [Start Counter_example2]
import { hiTraceMeter } from '@kit.PerformanceAnalysisKit';
import { common } from '@kit.AbilityKit';

@Entry
@Component
struct Index {
  @State message: string = 'getStringSync';
  private context = this.getUIContext().getHostContext() as common.UIAbilityContext;

  aboutToAppear(): void {
    hiTraceMeter.startTrace('getStringSync', 1);
    // The input parameter of the getStringSync interface uses the resource directly, without using the resource ID.
    this.context.resourceManager.getStringSync($r('app.string.app_name'));
    hiTraceMeter.finishTrace('getStringSync', 1);
  }

  build() {
    RelativeContainer() {
      Text(this.message)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
    }
    .height('100%')
    .width('100%')
  }
}

// [End Counter_example2]