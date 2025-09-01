// [Start create_node_controller]
// Create NodeController
// common.ets
import { UIContext } from '@kit.ArkUI';
import { webview } from '@kit.ArkWeb';
import { NodeController, BuilderNode, Size, FrameNode }  from '@kit.ArkUI';

// Specific component contents of dynamic components in @Builder
// Data is a parameter encapsulation class.
class Data{
  url: string = 'https://www.example.com';
  controller: WebviewController = new webview.WebviewController();
}

let shouldInactive: boolean = true;
@Builder
function WebBuilder(data:Data) {
  Column() {
    Web({ src: data.url, controller: data.controller })
      .domStorageAccess(true)
      .zoomAccess(true)
      .fileAccess(true)
      .mixedMode(MixedMode.All)
      .width('100%')
      .height('100%')
      .onPageBegin(() => {
        data.controller.onActive();
      })
      .onPageEnd(() => {
        console.info(`load page end time: ${Date.now()}`);
      })
      .onFirstMeaningfulPaint(() =>{
        if (!shouldInactive) {
          return;
        }
        // stop render
        data.controller.onInactive();
        shouldInactive = false;
      })
  }
}

let wrap = wrapBuilder<Data[]>(WebBuilder);

// Used to control and feedback the behavior of nodes on the corresponding NodeContainer, which needs to be used together with NodeContainer
export class myNodeController extends NodeController {
  private rootNode: BuilderNode<Data[]> | null = null;
  private root: FrameNode | null = null;

  // The method that must be overridden is used to build the number of nodes and return the nodes to be mounted in the corresponding NodeContainer.
  // //Called when the corresponding NodeContainer is created, or refreshed by calling the rebuild method.
  makeNode(uiContext: UIContext): FrameNode | null {
    console.log(' uicontext is undefined : '+ (uiContext === undefined));
    if (this.rootNode != null) {
      const parent = this.rootNode.getFrameNode()?.getParent();
      if (parent) {
        console.info(JSON.stringify(parent.getInspectorInfo()));
        parent.removeChild(this.rootNode.getFrameNode());
        this.root = null;
      }
      this.root = new FrameNode(uiContext);
      this.root.appendChild(this.rootNode.getFrameNode());
      // Returns the FrameNode node
      return this.root;
    }
    // Returns a null node that controls the dynamic component to be unbound.
    return null;
  }
  // Callback when layout size changes.
  aboutToResize(size: Size) {
    console.log('aboutToResize width : ' + size.width  +  ' height : ' + size.height )
  }

  // Call back when the NodeContainer corresponding to the controller is in Appear.
  aboutToAppear() {
    console.log('aboutToAppear')
  }

  // Call back when the NodeContainer corresponding to the controller is Disappear.
  aboutToDisappear() {
    console.log('aboutToDisappear')
  }

  // This function is a user-defined function and can be used as an initialization function.
  // Initialize builderNode through UIContext, and then initialize the contents in @Builder through the Build interface in BuilderNode.
  initWeb(url:string, uiContext:UIContext, control:WebviewController) {
    if(this.rootNode != null)
    {
      return;
    }
    // Creating a node requires uiContext.
    this.rootNode = new BuilderNode(uiContext)
    // Create dynamic Web components
    this.rootNode.build(wrap, { url:url, controller:control })
  }
}

// Create the NodeController needed for Map saving.
let NodeMap:Map<string, myNodeController | undefined> = new Map();
// Create WebViewController needed for Map saving.
let controllerMap:Map<string, WebviewController | undefined> = new Map();

// Initialization requires UIContext to be obtained in Ability.
export const createNWeb = (url: string, uiContext: UIContext) => {
  // Create NodeController
  let baseNode = new myNodeController();
  let controller = new webview.WebviewController() ;
  // Initialize a custom web component
  baseNode.initWeb(url, uiContext, controller);
  controllerMap.set(url, controller)
  NodeMap.set(url, baseNode);
}

// Customize to get the NodeController interface.
export const getNWeb = (url : string) : myNodeController | undefined => {
  return NodeMap.get(url);
}
// [End create_node_controller]