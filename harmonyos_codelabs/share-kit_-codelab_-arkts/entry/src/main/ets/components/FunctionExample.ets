/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

import { common } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { image } from '@kit.ImageKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { systemShare } from '@kit.ShareKit';
import SharedRecord from './modules/SharedRecord';
import ShareControllerOptions from './modules/ShareControllerOptions';
import Logger from '../utils/Logger';
import MoreRecord from './modules/MoreRecord';
import CreateRecord from '../utils/CreateRecord';

let logger = Logger.getLogger('[FunctionExample]');

@Component
export default struct FunctionDemo {
  @State currentIndex: number = 0;
  @State isToggleOn: boolean = false;
  @State isShowSheet: boolean = false;

  @State utd: string = utd.UniformDataType.TEXT;
  @State title: string = 'Test title';
  @State label: string = 'Test label';
  @State description: string = 'Test description';
  @State imageSource: image.ImageSource | null = null;
  @State uri: string = '';
  @State content: string = 'Test content';
  @State extraData: Record<string, string | number | boolean | Array<string | number | boolean>> = {};

  @State utd2: string = utd.UniformDataType.TEXT;
  @State title2: string = '';
  @State label2: string = '';
  @State description2: string = '';
  @State imageSource2: image.ImageSource | null = null;
  @State uri2: string = '';
  @State content2: string = '';
  @State extraData2: Record<string, string | number | boolean | Array<string | number | boolean>> = {};

  @State utdList: string[] = [];

  @State previewMode: systemShare.SharePreviewMode = systemShare.SharePreviewMode.DEFAULT;
  @State selectionMode: systemShare.SelectionMode = systemShare.SelectionMode.SINGLE;
  @State excludedAbilities: Array<systemShare.ShareAbilityType> = [];

  @Builder tabBuilder(index: number, name: ResourceStr, hasToggle: boolean = false) {
    Row() {
      Text(name)
        .margin({ right: 6 })
        .fontSize(18)
        .fontWeight(400)
        .fontColor(this.currentIndex === index ? $r('sys.color.brand') : $r('sys.color.font_primary'))
      if (hasToggle) {
        Toggle({ type: ToggleType.Switch, isOn: $$this.isToggleOn })
          .selectedColor($r('sys.color.brand'))
          .switchPointColor($r('sys.color.comp_background_list_card'))
          .onChange(async (isOn: boolean) => {
            if (isOn) {
              const context: Context = getContext(this);
              const filterRecord = await CreateRecord.filterRecordByUtd(context, this.utd2);
              if (filterRecord) {
                this.title2 = filterRecord.title ?? '';
                this.label2 = filterRecord.label ?? '';
                this.description2 = filterRecord.description ?? '';
                this.uri2 = filterRecord.uri ?? '';
                this.content2 = filterRecord.content ?? '';
              }
            }
          })
      }
    }
    .width('100%')
    .alignItems(VerticalAlign.Center)
    .justifyContent(FlexAlign.Center)
  }

  async createSharedRecord(): Promise<systemShare.SharedData> {
    let sharedRecordParams: systemShare.SharedRecord = {
      utd: this.utd,
      uri: this.uri,
      content: this.content,
      label: this.label,
      title: this.title,
      description: this.description,
    }
    if (this.imageSource) {
      const imagePacker: image.ImagePacker = image.createImagePacker();
      const buffer: ArrayBuffer = await imagePacker.packToData(this.imageSource, {
        format: 'image/jpeg',
        quality: 1,
      });
      sharedRecordParams.thumbnail = new Uint8Array(buffer);
    }
    return new systemShare.SharedData(sharedRecordParams);
  }

  async addSharedRecord(shareData: systemShare.SharedData): Promise<void> {
    let sharedRecordParams: systemShare.SharedRecord = {
      utd: this.utd2,
      uri: this.uri2,
      content: this.content2,
      label: this.label2,
      title: this.title2,
      description: this.description2,
    }
    if (this.imageSource2) {
      const imagePackerApi: image.ImagePacker = image.createImagePacker();
      const buffer: ArrayBuffer = await imagePackerApi.packing(this.imageSource2, {
        format: 'image/jpeg',
        quality: 1,
      });
      sharedRecordParams.thumbnail = new Uint8Array(buffer);
    }
    shareData.addRecord(sharedRecordParams);
  }

  async moreSharedRecord(shareData: systemShare.SharedData): Promise<void> {
    for (let index = 0; index < this.utdList.length; index++) {
      const utd = this.utdList[index];
      const context: Context = getContext(this);
      const filterRecord = await CreateRecord.filterRecordByUtd(context, utd);
      if (filterRecord) {
        shareData.addRecord(filterRecord);
      }
    }
  }

  async handelShare() {
    let shareData = await this.createSharedRecord();
    if (this.isToggleOn) {
      await this.addSharedRecord(shareData);
      await this.moreSharedRecord(shareData);
    }
    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      previewMode: this.previewMode,
      selectionMode: this.selectionMode,
      excludedAbilities: Array.from(this.excludedAbilities),
    }).then(() => {
      logger.info('controller show');
    }).catch((error: BusinessError) => {
      logger.error(`controller show error. Code: ${error.code}, message: ${error.message}`);
    });
  }

  build() {
    Flex({ direction: FlexDirection.Column }) {
      Column() {
        Tabs({ barPosition: BarPosition.Start, index: this.currentIndex }) {
          TabContent() {
            SharedRecord({
              utd: this.utd,
              title: this.title,
              label: this.label,
              description: this.description,
              imageSource: this.imageSource,
              uri: this.uri,
              content: this.content,
              extraData: this.extraData
            })
          }
          .tabBar(this.tabBuilder(0, $r('app.string.tab_text_1')))

          TabContent() {
            SharedRecord({
              utd: this.utd2,
              title: this.title2,
              label: this.label2,
              description: this.description2,
              imageSource: this.imageSource2,
              uri: this.uri2,
              content: this.content2,
              extraData: this.extraData2
            })
          }
          .tabBar(this.tabBuilder(1, $r('app.string.tab_text_2'), true))

          if (this.isToggleOn) {
            TabContent() {
              MoreRecord({
                utdList: this.utdList
              })
            }
            .tabBar(this.tabBuilder(2, $r('app.string.tab_text_3')))
          }
        }
        .vertical(false)
        .onChange((index: number) => {
          this.currentIndex = index;
        })
      }
      .width('100%')
      .flexGrow(1)
      .padding({ left: 20, right: 20 })

      ShareControllerOptions({
        previewMode: this.previewMode,
        selectionMode: this.selectionMode,
        excludedAbilities: this.excludedAbilities
      })

      Column() {
        Button($r('app.string.share_btn'))
          .width('100%')
          .height(40)
          .onClick(() => this.handelShare())
      }
      .height(100)
      .padding({ top: 10, left: 20, right: 20 })
    }
    .width('100%')
    .height('100%')
  }
}

