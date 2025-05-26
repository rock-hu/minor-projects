// [Start Case1_start]
@Observed
class Translate {
  translateX: number = 20;
}


@Entry
@Component
struct UnnecessaryState1 {
  @State translateObj: Translate = new Translate(); // If there are both read and write operations and a Button component is associated with it, it is recommended to use state variables.
  buttonMsg = 'I am button'; // Only read the value of the variable buttonMsg, without any write operations, just use the general variables directly


  build() {
    Column() {
      Button(this.buttonMsg)
        .onClick(() => {
          animateTo({
            duration: 50
          }, () => {
            this.translateObj.translateX = (this.translateObj.translateX + 50) % 150; // Reassign value to variable translateObj when clicked.
          })
        })
    }
    .translate({
      x: this.translateObj.translateX // Retrieve the value in translateObj.
    })
  }
}
// [End Case1_start]