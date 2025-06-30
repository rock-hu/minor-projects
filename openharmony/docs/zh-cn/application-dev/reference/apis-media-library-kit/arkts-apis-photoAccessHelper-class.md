# Classes (其他)

> **说明：**
>
> 本模块首批接口从API version 10开始支持。后续版本的新增接口，采用上角标单独标记接口的起始版本。

## RecommendationOptions<sup>11+</sup>

图片推荐选项(基于图片数据分析结果，依赖设备适配)。

**系统能力**：SystemCapability.FileManagement.PhotoAccessHelper.Core

| 名称                    | 类型                | 必填 | 说明                          |
| ----------------------- | ------------------- | ---- | -------------------------------- |
| recommendationType | [RecommendationType](arkts-apis-photoAccessHelper-e.md#recommendationtype11)   | 否   | 如果需要根据枚举值推荐相应的图片，则配置此参数。 <br>**原子化服务API：** 从API version 11开始，该接口支持在原子化服务中使用。|
| textContextInfo<sup>12+</sup> | [TextContextInfo](arkts-apis-photoAccessHelper-i.md#textcontextinfo12)   | 否   | 如果需要根据文本信息推荐相应的图片，则配置此参数(如果同时配置了recommendationType，则仅textContextInfo生效)。<br>**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。 |

## BaseSelectOptions<sup>12+</sup>

图库选择选项基类。

**系统能力**：SystemCapability.FileManagement.PhotoAccessHelper.Core

| 名称                    | 类型                | 必填 | 说明                          |
| ----------------------- | ------------------- | ---- | -------------------------------- |
| MIMEType<sup>10+</sup>    | [PhotoViewMIMETypes](arkts-apis-photoAccessHelper-e.md#photoviewmimetypes)   | 否   | 可选择的媒体文件类型，若无此参数，则默认为图片和视频类型。<br>**原子化服务API：** 从API version 11开始，该接口支持在原子化服务中使用。 |
| maxSelectNumber<sup>10+</sup>      | number | 否   | 选择媒体文件数量的最大值(最大可设置的值为500，若不设置则默认为50)。<br>**原子化服务API：** 从API version 11开始，该接口支持在原子化服务中使用。   |
| isPhotoTakingSupported<sup>11+</sup> | boolean  | 否   | 是否支持拍照，true表示支持，false表示不支持，默认为true。<br>**原子化服务API：** 从API version 11开始，该接口支持在原子化服务中使用。 |
| isSearchSupported<sup>11+</sup> | boolean  | 否   | 是否支持搜索，true表示支持，false表示不支持，默认为true。<br>**原子化服务API：** 从API version 11开始，该接口支持在原子化服务中使用。 |
| recommendationOptions<sup>11+</sup>       | [RecommendationOptions](#recommendationoptions11)   | 否   | 图片推荐相关配置参数。<br>**原子化服务API：** 从API version 11开始，该接口支持在原子化服务中使用。 |
| preselectedUris<sup>11+</sup> | Array&lt;string&gt;  | 否   | 预选择图片的uri数据。<br>**原子化服务API：** 从API version 11开始，该接口支持在原子化服务中使用。 |
| isPreviewForSingleSelectionSupported<sup>(deprecated)</sup> | boolean  | 否   | 单选模式下是否需要进大图预览，true表示需要，false表示不需要，默认为true。<br>**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。<br>从API version 12开始支持，从API version 18开始废弃。 |
| singleSelectionMode<sup>18+</sup> | [SingleSelectionMode](arkts-apis-photoAccessHelper-e.md#singleselectionmode18) | 否   | 单选模式类型。默认为大图预览模式（SingleSelectionMode.BROWSER_MODE）。<br>**原子化服务API：** 从API version 18开始，该接口支持在原子化服务中使用。 |
| mimeTypeFilter<sup>19+</sup> | [MimeTypeFilter](#mimetypefilter19)  | 否   | 文件类型的过滤配置，支持指定多个类型过滤。<br>当配置mimeTypeFilter参数时，MIMEType的配置自动失效。<br>配置该参数时，仅显示配置过滤类型对应的媒体文件，建议提示用户仅支持选择指定类型的图片/视频。<br>**原子化服务API：** 从API version 19开始，该接口支持在原子化服务中使用。 |
| fileSizeFilter<sup>19+</sup> | [FileSizeFilter](#filesizefilter19)  | 否   | 可选择媒体文件大小的过滤配置。<br>配置该参数时，仅显示配置文件大小范围的媒体文件，建议提示用户仅支持选择指定大小的图片/视频。<br>**原子化服务API：** 从API version 19开始，该接口支持在原子化服务中使用。 |
| videoDurationFilter<sup>19+</sup> | [VideoDurationFilter](#videodurationfilter19)  | 否   | 可选择媒体文件视频时长的过滤配置。<br>配置该参数时，仅显示配置视频时长范围的媒体文件，建议提示用户仅支持选择指定时长视频。<br>**原子化服务API：** 从API version 19开始，该接口支持在原子化服务中使用。 |

## PhotoSelectOptions

PhotoSelectOptions extends BaseSelectOptions

图库选择选项子类，继承自[BaseSelectOptions](#baseselectoptions12)。

**系统能力**：SystemCapability.FileManagement.PhotoAccessHelper.Core

| 名称                    | 类型                | 必填 | 说明                          |
| ----------------------- | ------------------- | ---- | -------------------------------- |
| isEditSupported<sup>11+</sup>       | boolean | 否   | 是否支持编辑照片，true表示支持，false表示不支持，默认为true。<br>**原子化服务API：** 从API version 11开始，该接口支持在原子化服务中使用。     |
| isOriginalSupported<sup>12+</sup>       | boolean | 否   | 是否显示选择原图按钮，true表示显示，false表示不显示，默认为true。<br>**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。     |
| subWindowName<sup>12+</sup>       | string | 否   | 子窗窗口名称。<br>**原子化服务API：** 从API version 12开始，该接口支持在原子化服务中使用。     |
| completeButtonText<sup>14+</sup>       | [CompleteButtonText](arkts-apis-photoAccessHelper-e.md#completebuttontext14) | 否   | 完成按钮显示的内容。<br>完成按钮指在界面右下方，用户点击表示图片选择已完成的按钮。 <br>**原子化服务API：** 从API version 14开始，该接口支持在原子化服务中使用。     |

## PhotoSelectResult

返回图库选择后的结果集。

**原子化服务API：** 从API version 11开始，该接口支持在原子化服务中使用。

**系统能力**：SystemCapability.FileManagement.PhotoAccessHelper.Core

| 名称                    | 类型                | 可读 | 可写 | 说明                           |
| ----------------------- | ------------------- | ---- | ---- | ------------------------------ |
| photoUris        | Array&lt;string&gt;    | 是   | 是   | 返回图库选择后的媒体文件的uri数组，此uri数组只能通过临时授权的方式调用[photoAccessHelper.getAssets接口](arkts-apis-photoAccessHelper-PhotoAccessHelper.md#getassets)去使用，具体使用方式参见用户文件uri介绍中的[媒体文件uri的使用方式](../../file-management/user-file-uri-intro.md#媒体文件uri的使用方式)。 |
| isOriginalPhoto        | boolean    | 是   | 是   | 返回图库选择后的媒体文件是否为原图。true为是原图，false为不是原图，默认值是false。 |

## MimeTypeFilter<sup>19+</sup>

文件类型的过滤配置。

**原子化服务API：** 从API version 19开始，该接口支持在原子化服务中使用。

**系统能力**：SystemCapability.FileManagement.PhotoAccessHelper.Core

| 名称                    | 类型                | 必填 | 说明                           |
| ----------------------- | ------------------- | ---- | ------------------------------ |
| mimeTypeArray        | Array&lt;string&gt;    | 是   | PhotoPicker可供用户选择媒体文件的过滤类型。最多支持指定十种媒体文件类型。<br>过滤类型参考MIME类型定义，例如：“image/jpeg”、“video/mp4”等。 |

## FileSizeFilter<sup>19+</sup>

可选择媒体文件大小的过滤配置。

**原子化服务API：** 从API version 19开始，该接口支持在原子化服务中使用。

**系统能力**：SystemCapability.FileManagement.PhotoAccessHelper.Core

| 名称                    | 类型                | 必填 | 说明                           |
| ----------------------- | ------------------- | ---- |------------------------------ |
| filterOperator        | [FilterOperator](arkts-apis-photoAccessHelper-e.md#filteroperator19)    | 是  | 过滤操作符。<br>例如：按照大于/小于某个fileSize的方式过滤文件。 |
| fileSize        | number    | 是 | 指定进行过滤的文件大小。<br>单位为字节（Byte）。 |
| extraFileSize   | number    | 否 | 针对FilterOperator.BETWEEN情况下，配置文件大小的上限值。默认值为-1。<br>单位为字节（Byte）。 |

## VideoDurationFilter<sup>19+</sup>

可选择媒体文件视频时长的过滤配置。

**原子化服务API：** 从API version 19开始，该接口支持在原子化服务中使用。

**系统能力**：SystemCapability.FileManagement.PhotoAccessHelper.Core

| 名称                    | 类型                | 必填 | 说明                           |
| ----------------------- | ------------------- | ---- |------------------------------ |
| filterOperator        | [FilterOperator](arkts-apis-photoAccessHelper-e.md#filteroperator19)    | 是  |过滤操作符。<br>例如：按照大于/小于某个videoDuration的方式过滤可选择的视频。 |
| videoDuration        | number    | 是 | 指定过滤视频的时长。<br>单位为毫秒（ms）。 |
| extraVideoDuration   | number    | 否 | 针对FilterOperator.BETWEEN情况下，配置视频时长的上限值。默认值为-1。<br>单位为毫秒（ms）。 |