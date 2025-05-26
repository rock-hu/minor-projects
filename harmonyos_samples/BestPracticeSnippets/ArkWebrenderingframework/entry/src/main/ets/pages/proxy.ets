import { webview } from '@kit.ArkWeb';
import { BuilderNode, window } from '@kit.ArkUI';

interface GeneratedObjectLiteralInterface_1 {
  webController: WebviewController;
}

class DynamicNodeManager {
  private rootNode: BuilderNode<[Object]> | null = null;
  private surfaceId: string = '';
  private renderType: number = 0;
  private embedType?: string;
  private componentId?: string;
  private nativeComponentBuilderMap;

  // 初始化必要参数
  constructor(params: {
    surfaceId: string,
    renderType: number,
    embedType?: string,
    componentId?: string
  }) {
    this.surfaceId = params.surfaceId;
    this.renderType = params.renderType;
    this.embedType = params.embedType;
    this.componentId = params.componentId;
  }

  // 更新配置参数
  updateConfig(params: {
    embedType?: string,
    componentId?: string
  }) {
    this.embedType = params.embedType ?? this.embedType;
    this.componentId = params.componentId ?? this.componentId;
  }
  // [Start make_node]
  makeNode(uiContext: UIContext): FrameNode | null {
    if (this.rootNode === null) {
      return null;
    }
    if (!this.embedType || !this.componentId) {
      return null;
    }

    if (!this.rootNode) {
      this.rootNode = new BuilderNode<[Object]>(uiContext, {
        surfaceId: this.surfaceId,
        type: this.renderType
      });

      if (this.nativeComponentBuilderMap.get(this.embedType)) {
        const wrapBuilder: WrappedBuilder<[Object]> = new WrappedBuilder(this.nativeComponentBuilderMap.get(this.embedType) as (...args: Object[]) => void);
        this.rootNode.build(wrapBuilder, sameLayerArgsMap.get(this.componentId));
      } else {
        return null;
      }
    }

    return this.rootNode.getFrameNode();
  }
  // [End make_node]
}



// 需实现ChannelService类并初始化实例
class ChannelService {
  call<T>(channelType: string, json: string): T {
    return JSON.parse(json) as T;
  }
}
class NativeBridge {
  private channelListeners: Map<string, Function> = new Map();

  // 注册通道处理器
  registerHandler(channelType: string, handler: Function): void {
    this.channelListeners.set(channelType, handler);
  }
  // [Start listener]
  // 执行通道调用
  call(channelType: string, objectJson: string): any {
    let fun = this.channelListeners.get(channelType);
    if (!fun) {
      return undefined;
    }
    let result :string= fun(JSON.parse(objectJson));
    return JSON.stringify(result);
  }
  // [End listener]
}

const ChannelInstance = new ChannelService()
@Component
struct Proxy
  private webController: webview.WebviewController = new webview.WebviewController();
  private webUrl: string = './Index';
  private taroWebController: GeneratedObjectLiteralInterface_1 = {
    webController: this.webController
  };
  // [Start call]
  nativeCall(channelType: string, object: string) {
    let  objectJson = JSON.stringify(object);
    let resultJson :string= window.JSBridge && window.JSBridge.nativeMethod(channelType, objectJson);
    return resultJson && JSON.parse(resultJson);
  }
  // [End call]


  // [Start build_web]
  build() {
    Column() {
      Web({
        src: this.webUrl,
        controller: this.webController
      }).javaScriptProxy({
        object: {
          nativeMethod: (channelType: string, objectJson: string) => {
            return ChannelInstance.call(channelType, objectJson) as object;
          }
        },
        name: 'JSBridge',
        methodList: ['nativeMethod'],
        controller: this.taroWebController.webController
      })
    }
    // [Ends build_web]
  }

}
