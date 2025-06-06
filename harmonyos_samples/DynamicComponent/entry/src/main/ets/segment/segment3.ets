// [Start segment3_start]
// [StartExclude segment3_start]
import { BuilderNode, FrameNode, NodeController } from '@kit.ArkUI';

class Params {
  text: string = 'Hello World';

  constructor(text: string) {
    this.text = text;
  }
}

@Builder
function buildText(params: Params) {
  Column() {
    Text(params.text)
      .fontSize(50)
      .fontWeight(FontWeight.Bold)
      .margin({ bottom: 36 })
  }
}

class TextNodeController extends NodeController {
  private textNode: BuilderNode<[Params]> | null = null;
  private message: string = '';

  constructor(message: string) {
    super();
    this.message = message;
  }

  makeNode(context: UIContext): FrameNode | null {
    // Creating a BuilderNode instance
    this.textNode = new BuilderNode(context);
    // Set the selfIdealSize property
    // this.textNode = new BuilderNode(context, {selfIdealSize: {width: 100, height :100}});
    // Build the component tree using the build method
    this.textNode.build(wrapBuilder<[Params]>(buildText), new Params(this.message));
    // Returns the node to be displayed
    return this.textNode.getFrameNode();
  }
}
// [EndExclude segment3_start]
@Entry
@Component
struct Index {
  @State message: string = "hello";
  private textNodeController: TextNodeController = new TextNodeController(this.message);

  build() {
    Row() {
      Column() {
        NodeContainer(this.textNodeController)
          .width('100%')
          .height(100)
          .backgroundColor('#FFF0F0F0')
      }
      .width('100%')
      .height('100%')
    }
    .height('100%')
  }
}
// [End segment3_start]