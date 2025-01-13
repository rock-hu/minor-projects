/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
import { systemShare } from '@kit.ShareKit';
import CreateRecord from '../utils/CreateRecord';
import PostersScenario from '../scenario/PostersScenario';
import ImageScenario from '../scenario/ImageScenario';
import VideoScenario from '../scenario/VideoScenario';
import TextScenario from '../scenario/TextScenario';
import AppLinkingScenario from '../scenario/AppLinkingScenario';
import LinkScenario from '../scenario/LinkScenario';

@Component
export default struct UTDDemo {
  @State SelectOption: SelectOption[] = CreateRecord.utdMap.map((item) => {
    return { value: item.utd } as SelectOption;
  })

  @State text: ResourceStr = this.SelectOption[0].value;
  @State index: number = 0;

  @State records: systemShare.SharedRecord[] = [];

  build() {
    Scroll() {
      Column() {
        AppLinkingScenario()
        LinkScenario()
        ImageScenario()
        VideoScenario()
        TextScenario()
        PostersScenario()
      }
      .width('100%')
      .constraintSize({ minHeight: '100%' })
      .padding({ left: 20, right: 20 })
    }
    .width('100%')
    .height('100%')
  }
}