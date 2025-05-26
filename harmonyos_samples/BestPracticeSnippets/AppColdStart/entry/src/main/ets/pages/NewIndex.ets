// [Start new_index]
// Index.ets
import { number } from '../utils/Calculator';

AppStorage.link('netData');
PersistentStorage.persistProp('netData', undefined);

@Entry
@Component
struct Index {
  @State message: string = 'Hello World' + number; // In order to reflect the performance benefits, refer to the execution result number of the time-consuming function.
  @StorageLink('netData') netData: PixelMap | undefined = undefined;
  build() {
    Row() {
      Image(this.netData)
        .objectFit(ImageFit.Contain)
        .width('50%')
        .height('50%')
    }
    .onDisAppear(() => {
      AppStorage.set('netData', undefined);
    })
    .height('100%')
    .width('100%')
  }
}
// [End new_index]