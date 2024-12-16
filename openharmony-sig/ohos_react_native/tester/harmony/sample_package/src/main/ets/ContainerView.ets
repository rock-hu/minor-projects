import {
  Descriptor,
  ViewBaseProps,
  RNViewBase,
  RNComponentContext,
} from '@rnoh/react-native-openharmony';


interface ContainerViewProps extends ViewBaseProps {
  size: number
}

export type ContainerViewDescriptor = Descriptor<"ContainerView", ContainerViewProps>

@Component
export struct ContainerView {
  public static readonly NAME = "ContainerView"
  public ctx!: RNComponentContext
  public tag: number = 0

  @State descriptor: ContainerViewDescriptor = {} as ContainerViewDescriptor
  private cleanUpCallbacks: (() => void)[] = []

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<ContainerViewDescriptor>(this.tag)
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.descriptor = (newDescriptor as ContainerViewDescriptor)
      }
    ))
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cb => cb())
  }

  build() {
    Stack() {
      RNViewBase({ tag: this.tag, ctx: this.ctx }) {
        ContentSlot(this.ctx.getContentForTag(this.tag))
      }
      Text("Custom Container").alignSelf(ItemAlign.Center)
    }
  }
}
