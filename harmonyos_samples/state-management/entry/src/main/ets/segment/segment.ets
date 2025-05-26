// [Start Counter_example1_start]
@Observed
class Translate {
  translateX: number = 20;
}
@Entry
@Component
struct MyComponent {
  @State translateObj: Translate = new Translate(); // The variable translateObj is not associated with any UI component and should not be defined as a state variable
  @State buttonMsg: string = 'I am button'; // The variable buttonMsg is not associated with any UI component and should not be defined as a state variable
  build() {
  }
}
// [End Counter_example1_start]