# OhosImageSourceInfo


## Overview

The **OhosImageSourceInfo** struct defines the image source information, which is obtained by calling [OH_ImageSource_GetImageInfo](image.md#oh_imagesource_getimageinfo).

**System capability**: SystemCapability.Multimedia.Image

**Since**: 10

**Related module**: [Image](image.md)

**Header file**: [image_source_mdk.h](image__source__mdk_8h.md)


## Summary


### Member Variables

| Name| Description| 
| -------- | -------- |
| int32_t [pixelFormat](image.md#pixelformat-33) | Pixel format of the image source. It is set in [OH_ImageSource_Create()](image.md#oh_imagesource_create).|
| int32_t [colorSpace](image.md#colorspace) | Color space of the image source.|
| int32_t [alphaType](image.md#alphatype) | Alpha type of the image source.|
| int32_t [density](image.md#density-22) | Image density of the image source. It is set in [OH_ImageSource_Create()](image.md#oh_imagesource_create).|
| struct [OhosImageSize](_ohos_image_size.md) [size](image.md#size-37) | Pixel width and height of the image source.|
