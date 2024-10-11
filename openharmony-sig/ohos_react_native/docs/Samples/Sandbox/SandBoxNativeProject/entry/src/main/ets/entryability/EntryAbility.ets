import { RNAbility } from '@rnoh/react-native-openharmony';
import window from '@ohos.window';

export default class EntryAbility extends RNAbility {
  getPagePath() {
    return 'pages/Index';
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    super.onWindowStageCreate(windowStage)
    AppStorage.setOrCreate('RNAbility', this)
  }
}
