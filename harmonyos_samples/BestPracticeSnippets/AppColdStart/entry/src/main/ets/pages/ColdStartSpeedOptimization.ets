// [Start cold_start_speed_optimization]
const LARGE_NUMBER = 100000000;
const DELAYED_TIME = 1000;

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  aboutToAppear(): void {
    console.log('aboutToAppear');
    this.computeTaskAsync();
  }

  // [StartExclude cold_start_speed_optimization]
  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
    }
    .height('100%')
  }
  // [EndExclude cold_start_speed_optimization]

  computeTask(): void {
    let count = 0;
    while (count < LARGE_NUMBER) {
      count++;
    }
  }

  // 运算任务异步处理
  private computeTaskAsync(): void {
    setTimeout(() => { // 这里使用setTimeout来实现异步延迟运行
      this.computeTask();
    }, DELAYED_TIME);
  }
}
// [End cold_start_speed_optimization]