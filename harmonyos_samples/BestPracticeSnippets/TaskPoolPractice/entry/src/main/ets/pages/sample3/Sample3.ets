import { taskpool } from '@kit.ArkTS';
import { emitter } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

// [Start wrong_1]
// Sample3.ets
@Concurrent
function wrongConcurrentFunc() {
  let promise = Promise.resolve();
  promise.then(() => {
    taskpool.Task.sendData();
  })
}
// [End wrong_1]

// [Start correct_fun_1]
// Sample3.ets
@Concurrent
function correctConcurrentFunc() {
  let promise = Promise.resolve();
  promise.then(() => {
    emitter.emit("1", {data: {name: "anonymous"}});
  })
}
// [End correct_fun_1]

@Entry
@Component
struct Sample3 {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column({ space: 12 }) {
        Blank()
        Button('Wrong Sample')
          .height(40)
          .width('100%')
          .onClick(() => {
            // [Start task_pool_1]
            // Sample3.ets
            let task: taskpool.Task = new taskpool.Task(wrongConcurrentFunc);
            task.onReceiveData(() => {
              hilog.info(0xFF00, 'sampleTag', "onReceiveData has been called");
            })
            taskpool.execute(task);
            // [End task_pool_1]
          })

        Button('Correct Sample')
          .height(40)
          .width('100%')
          .onClick(() => {
            // [Start task_7]
            // Sample3.ets
            let task: taskpool.LongTask = new taskpool.LongTask(correctConcurrentFunc);
            emitter.on("1", (data: emitter.EventData) => {
              hilog.info(0xff00, 'sampleTag', "name is : " + data.data?.name);
              emitter.off("1")
              taskpool.terminateTask(task)
            })
            taskpool.execute(task);
            // [End task_7]
          })
      }
      .height('100%')
      .width('100%')
      .padding(16)
      .justifyContent(FlexAlign.Center)
    }
    .height('100%')
  }
}