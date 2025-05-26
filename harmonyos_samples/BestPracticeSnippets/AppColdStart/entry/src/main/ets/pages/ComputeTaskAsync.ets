// [Start compute_task_async]
const LARGE_NUMBER = 100000000;
const DELAYED_TIME = 1000;

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  aboutToAppear(): void {
    // Time-consuming operation
    // this.computeTask();
    this.computeTaskAsync(); // Asynchronous tasks
  }

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

  private computeTask(): void {
    let count = 0;
    while (count < LARGE_NUMBER) {
      count++;
    }
  }

  // Asynchronous processing of computing tasks
  private computeTaskAsync(): void {
    setTimeout(() => { // SetTimeout is used here to achieve asynchronous delayed operation.
      this.computeTask();
    }, DELAYED_TIME);
  }
}
// [End compute_task_async]