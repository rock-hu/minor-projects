// [Start animate_to2]

class MyData {
  str: string;
  constructor(str: string) {
    this.str = str;
  }
}
@Entry
@Component
struct Index {
  @State data1: MyData|undefined = new MyData("branch 0");
  @State data2: MyData|undefined = new MyData("branch 1");
  build() {
    Column() {
      if (this.data1) {
        // Display a specified empty transition effect in the root component of IfElse to avoid default transition animations
        Text(this.data1.str)
          .transition(TransitionEffect.IDENTITY)
          .id("1")
      } else if (this.data2) {
        // Display a specified empty transition effect in the root component of IfElse to avoid default transition animations
        Text(this.data2.str)
          .transition(TransitionEffect.IDENTITY)
          .id("2")
      }
      Button("play with animation")
        .onClick(() => {
          this.getUIContext().animateTo({}, ()=>{
            // Modifying the if condition in animateTo will give default transitions to the first layer components under if in the animation
            // But since the specified transition has already been displayed, no default transition will be added
            if (this.data1) {
              this.data1 = undefined;
              this.data2 = new MyData("branch 1");
            } else {
              this.data1 = new MyData("branch 0");
              this.data2 = undefined;
            }
          })
        })
    }.width("100%")
    .padding(10)
  }
}
// [End animate_to2]