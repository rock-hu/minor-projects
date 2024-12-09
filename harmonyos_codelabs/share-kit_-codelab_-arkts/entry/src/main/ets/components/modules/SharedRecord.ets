/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
import { image } from '@kit.ImageKit';
import { photoAccessHelper } from '@kit.MediaLibraryKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { picker } from '@kit.CoreFileKit';
import { dataSharePredicates } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import FormItem from './FormItem';
import CreateRecord from '../../utils/CreateRecord';

@Component
export default struct SharedRecord {
  @Link utd: string;
  @Link title: string;
  @Link label: string;
  @Link description: string;
  @Link pixelMap: image.PixelMap | null;
  @Link uri: string;
  @Link content: string;
  @Link extraData: Record<string, string | number | boolean | Array<string | number | boolean>>;

  @State SelectOption: SelectOption[] = CreateRecord.utdMap.map((item) => {
    return { value: item.utd } as SelectOption;
  })

  @Builder
  UTDBuilder() {
    Select(this.SelectOption)
      .value(this.utd)
      .onSelect(async (index: number, text?: string | undefined) => {
        if (text) {
          this.utd = text;
          const context: Context = getContext(this);
          const recordData = await CreateRecord.utdMap[index].action(context);
          this.title = recordData.title ?? '';
          this.label = recordData.label ?? '';
          this.description = recordData.description ?? '';
          this.uri = recordData.uri ?? '';
          this.content = recordData.content ?? '';
        }
      })
  }

  @Builder
  TitleBuilder() {
    TextInput({
      text: this.title,
      placeholder: 'Enter a title.'
    })
      .textAlign(TextAlign.End)
      .placeholderColor(Color.Grey)
      .placeholderFont({ size: 14 })
      .fontSize(14)
      .fontColor(Color.Black)
      .onChange((value: string) => this.title = value)
  }

  @Builder
  LabelBuilder() {
    TextInput({
      text: this.label,
      placeholder: 'Enter a label.'
    })
      .textAlign(TextAlign.End)
      .placeholderColor(Color.Grey)
      .placeholderFont({ size: 14 })
      .fontSize(14)
      .fontColor(Color.Black)
      .onChange((value: string) => this.label = value)
  }

  @Builder
  DescriptionBuilder() {
    TextInput({
      text: this.description,
      placeholder: 'Enter a description.'
    })
      .textAlign(TextAlign.End)
      .placeholderColor(Color.Grey)
      .placeholderFont({ size: 14 })
      .fontSize(14)
      .fontColor(Color.Black)
      .onChange((value: string) => this.description = value)
  }

  @Builder
  ThumbnailBuilder() {
    Button(this.pixelMap ? $r('app.string.selected') : $r('app.string.select'))
      .onClick(() => {
        let context = getContext(this) as common.Context;
        let photoPicker = new picker.PhotoViewPicker(context);
        photoPicker.select({
          MIMEType: picker.PhotoViewMIMETypes.IMAGE_TYPE,
          maxSelectNumber: 1,
        }).then(async (photoSelectResult: picker.PhotoSelectResult) => {
          let photoUri = photoSelectResult.photoUris;
          let phAccessHelper = photoAccessHelper.getPhotoAccessHelper(context);
          let predicates: dataSharePredicates.DataSharePredicates = new dataSharePredicates.DataSharePredicates();
          predicates.equalTo('uri', photoUri[0]);
          let fetchOption: photoAccessHelper.FetchOptions = {
            fetchColumns: [],
            predicates: predicates
          };
          let fetchResult: photoAccessHelper.FetchResult<photoAccessHelper.PhotoAsset> = await phAccessHelper.getAssets(fetchOption);
          const asset: photoAccessHelper.PhotoAsset = await fetchResult.getFirstObject();
          this.pixelMap = await asset.getThumbnail();
        }).catch((err: BusinessError) => {
          console.error(`PhotoViewPicker.select failed with err: Code: ${err.code}. message: ${err.message}`);
        });
      })
  }

  @Builder
  UriBuilder() {
    TextInput({
      text: this.uri,
      placeholder: 'Enter a URI.'
    })
      .textAlign(TextAlign.End)
      .placeholderColor(Color.Grey)
      .placeholderFont({ size: 14 })
      .fontSize(14)
      .fontColor(Color.Black)
      .onChange((value: string) => this.uri = value)
  }

  @Builder
  ContentBuilder() {
    TextInput({
      text: this.content,
      placeholder: 'Enter content.'
    })
      .textAlign(TextAlign.End)
      .placeholderColor(Color.Grey)
      .placeholderFont({ size: 14 })
      .fontSize(14)
      .fontColor(Color.Black)
      .onChange((value: string) => this.content = value)
  }

  build() {
    Column() {
      FormItem({ label: 'utd', content: (): void => this.UTDBuilder()});
      FormItem({ label: 'title', content: (): void => this.TitleBuilder()});
      FormItem({ label: 'label', content: (): void => this.LabelBuilder()});
      FormItem({ label: 'description', content: (): void => this.DescriptionBuilder()});
      FormItem({ label: 'thumbnail', content: (): void => this.ThumbnailBuilder()});
      FormItem({ label: 'uri', content: (): void => this.UriBuilder()});
      FormItem({ label: 'content', content: (): void => this.ContentBuilder()});
    }
    .width('100%')
    .height('100%')
  }
}