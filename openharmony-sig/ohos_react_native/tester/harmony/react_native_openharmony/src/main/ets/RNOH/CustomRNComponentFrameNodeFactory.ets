/**
 * Only used in C-API architecture.
 */
import { BuilderNode, FrameNode, UIContext } from '@kit.ArkUI';
import { Tag, Descriptor } from "./DescriptorBase"
import { RNComponentContext, ComponentBuilderContext } from "./EtsRNOHContext"
import type { FrameNodeFactory } from "./RNInstance"
import { RNOHLogger } from "./RNOHLogger"

interface CustomRNComponentFrameNodeFactoryResult {
  frameNode: FrameNode
  destroyBuilderNode: () => void
}

/**
 * @internal
 */
export class CustomRNComponentFrameNodeFactory implements FrameNodeFactory {
  private uiCtx: UIContext;
  private getRNComponentContext: () => (RNComponentContext | undefined)
  private componentContext: RNComponentContext | undefined;
  private logger: RNOHLogger

  constructor(uiCtx: UIContext, logger: RNOHLogger, getRNComponentContext: () => (RNComponentContext | undefined)) {
    this.uiCtx = uiCtx;
    this.getRNComponentContext = getRNComponentContext;
    this.logger = logger.clone("CustomRNComponentFrameNodeFactory")
  }

  create(tag: Tag, componentName: string): CustomRNComponentFrameNodeFactoryResult {
    const logger = this.logger.clone("create")
    const stopTracing = logger.startTracing()
    if (this.componentContext === undefined) {
      this.componentContext = this.getRNComponentContext();
      logger.fatal("ComponentContext is undefined")
    }
    const builderCtx = new ComponentBuilderContext(
      this.componentContext!,
      this.componentContext!,
      tag,
      componentName,
      this.componentContext!.descriptorRegistry.getDescriptor<Descriptor>(tag),
    );
    const builderNode = new BuilderNode<[ComponentBuilderContext]>(this.uiCtx);
    builderNode.build(this.componentContext!.wrappedCustomRNComponentBuilder, builderCtx);
    const frameNode = builderNode.getFrameNode();
    if (frameNode === null) {
      this.componentContext!.logger.fatal(`Couldn't create frameNode for: ${componentName}`);
    }
    stopTracing();
    return { frameNode: frameNode!, destroyBuilderNode: () => {
      builderNode.dispose()
    } };
  }
}
