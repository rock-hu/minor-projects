import { RNComponentContext, RNViewBase } from '@rnoh/react-native-openharmony';
import { CodegenLibArkTSSampleComponent as GENERATED } from "./generated/components/CodegenLibArkTSSampleComponent"

@Component
export struct CodegenLibSampleArkTSComponent {
  public static readonly NAME = GENERATED.NAME
  public ctx!: RNComponentContext
  public tag: number = 0
  @State private descriptorWrapper: GENERATED.DescriptorWrapper = {} as GENERATED.DescriptorWrapper
  private eventEmitter: GENERATED.EventEmitter | undefined = undefined
  private cleanUpCallbacks: (() => void)[] = []

  aboutToAppear() {
    this.eventEmitter = new GENERATED.EventEmitter(this.ctx.rnInstance, this.tag)
    this.onDescriptorWrapperChange(this.ctx.descriptorRegistry.findDescriptorWrapperByTag<GENERATED.DescriptorWrapper>(this.tag)!)
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (_descriptor, newDescriptorWrapper) => {
        this.onDescriptorWrapperChange(newDescriptorWrapper! as GENERATED.DescriptorWrapper)
      }
    ))
    setTimeout(() => {
      this.eventEmitter!.emit("mount", {
        text: this.ctx.descriptorRegistry.findDescriptorWrapperByTag<GENERATED.DescriptorWrapper>(this.tag)!.props.text
      });
    }, 1000)

  }

  private onDescriptorWrapperChange(descriptorWrapper: GENERATED.DescriptorWrapper) {
    this.descriptorWrapper = descriptorWrapper
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cb => cb())
  }

  build() {
    RNViewBase({ ctx: this.ctx, tag: this.tag }) {
      Text(this.descriptorWrapper.props.text).width("100%").height("100%")
      this.ctx.wrappedRNChildrenBuilder.builder(this.ctx, this.tag)
    }
  }
}
