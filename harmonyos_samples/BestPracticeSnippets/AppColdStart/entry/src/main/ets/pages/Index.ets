// [Start request_in_home]
// entry/src/main/ets/pages/Index.ets
import { httpRequest } from '../utils/NetRequest';

import { number } from '../utils/Calculator';

AppStorage.link('netData');
PersistentStorage.persistProp('netData', undefined);

@Entry
@Component
struct Index {
  @State message: string = 'Hello World' + number; // In order to reflect the performance benefits, refer to the execution result number of the time-consuming function.
  @StorageLink('netData') netData: PixelMap | undefined = undefined;
  build(){
    Row(){
      Image(this.netData)
        .objectFit(ImageFit.Contain)
        .width('50%')
        .height('50%')
    }
    .onAppear(() => {
      // Send a network request
      httpRequest();
    })
  }
}
// [End request_in_home]
