/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start taskpool_failed_to_serialize_result_case]
import taskpool from '@ohos.taskpool'
import {printArgs} from './utils'
import {BusinessError} from '@ohos.base'
@Concurrent
function createTask(a:number, b:number) {
  let sum = a + b;
  let task:taskpool.Task = new taskpool.Task(printArgs, sum);
  return task; //返回值task任务是无法被序列化，导致问题产生
}
function resultError() {
  let task: taskpool.Task = new taskpool.Task(createTask, 1, 2);
  taskpool.execute(task).then((res)=>{
  }).catch((e:BusinessError)=>{
    console.error("resultError task catch message:" + e.message)
  })
}

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    RelativeContainer() {
      Text(this.message)
        .id('HelloWorld')
        .fontSize($r('app.float.page_text_font_size'))
        .fontWeight(FontWeight.Bold)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .onClick(() => {
          this.message = 'Welcome';
        })
        Button("resultError")
        .onClick(() => {
          resultError()
        })
    }
    .height('100%')
    .width('100%')
  }
}
// [End taskpool_failed_to_serialize_result_case]