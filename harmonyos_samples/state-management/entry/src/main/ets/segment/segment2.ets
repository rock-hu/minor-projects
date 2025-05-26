// [Start Counter_example2_start]
@Observed
class Translate {
  translateX: number = 20;
}
@Entry
@Component
struct MyComponent {
  @State translateObj: Translate = new Translate();
  @State buttonMsg: string = 'I am button';
  build() {
    Column() {
      Button(this.buttonMsg) // Here we just read the value of the variable buttonMsg, without any write operation.
    }
  }
}
// [End Counter_example2_start]