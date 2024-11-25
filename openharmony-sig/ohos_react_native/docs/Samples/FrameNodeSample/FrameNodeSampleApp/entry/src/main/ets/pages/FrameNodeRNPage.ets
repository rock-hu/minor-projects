import { RNInstance, RNOHCoreContext } from '@rnoh/react-native-openharmony';
import { BaseRN } from '../rn';
import { LoadManager } from '../rn/LoadBundle';
import { Params } from './BuilderType';

@Component
export default struct FrameNodeRNPage {
  @State message: string = 'Hello World, FrameNodeRNPage';
  private instance: RNInstance = LoadManager.cpInstance
  private bundlePath = 'bundle/bundle.harmony.bundle'
  private moduleName = 'App'
  @StorageLink('isMetroAvailable') isMetroAvailable: boolean = false
  private params: Params = {
    text: ''
  };

  constructor(params: Params) {
    super();
    this.params = params;
  }

  aboutToAppear(): void {
  }

  aboutToDisappear(): void {
    const rnohCoreContext: RNOHCoreContext | undefined = AppStorage.get('RNOHCoreContext')
    rnohCoreContext?.destroyAndUnregisterRNInstance(this.instance)
  }

  build() {
    Stack() {
      BaseRN({
        rnInstance: this.instance,
        moduleName: this.moduleName,
        bundlePath: this.bundlePath,
      }).align(Alignment.Top).margin({ top: 0 })
    }
  }
}