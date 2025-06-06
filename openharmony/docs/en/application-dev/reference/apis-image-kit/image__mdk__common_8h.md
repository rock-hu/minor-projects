# image_mdk_common.h


## Overview

The **image_mdk_common.h** file declares the common enums and structs used by the image interface.

**Library**: libimage_ndk.z.so

**File to include**: &lt;multimedia/image_framework/image_mdk_common.h&gt;

**Since**: 10

**Related module**: [Image](image.md)


## Summary


### Structs

| Name| Description| 
| -------- | -------- |
| struct  [OhosImageSize](_ohos_image_size.md) | Defines the image size. | 


### Macros

| Name| Description| 
| -------- | -------- |
| **IMAGE_RESULT_BASE** 62980096 | Defines the basic value returned by the interface. |

### Types

| Name| Description| 
| -------- | -------- |
| [IRNdkErrCode](image.md#irndkerrcode) | Defines an enum that enumerates the return values that may be used by the interface.| 


### Enums

| Name| Description| 
| -------- | -------- |
| [IRNdkErrCode](image.md#irndkerrcode) {<br>IMAGE_RESULT_SUCCESS = 0, IMAGE_RESULT_BAD_PARAMETER = -1,<br>IMAGE_RESULT_IMAGE_RESULT_BASE = IMAGE_RESULT_BASE,<br>IMAGE_RESULT_ERR_IPC = IMAGE_RESULT_BASE + 1,<br>IMAGE_RESULT_ERR_SHAMEM_NOT_EXIST = IMAGE_RESULT_BASE + 2,<br>IMAGE_RESULT_ERR_SHAMEM_DATA_ABNORMAL = IMAGE_RESULT_BASE + 3,<br>IMAGE_RESULT_DECODE_ABNORMAL = IMAGE_RESULT_BASE + 4,<br>IMAGE_RESULT_DATA_ABNORMAL = IMAGE_RESULT_BASE + 5,<br>IMAGE_RESULT_MALLOC_ABNORMAL = IMAGE_RESULT_BASE + 6,<br>IMAGE_RESULT_DATA_UNSUPPORT = IMAGE_RESULT_BASE + 7,<br>IMAGE_RESULT_INIT_ABNORMAL = IMAGE_RESULT_BASE + 8,<br>IMAGE_RESULT_GET_DATA_ABNORMAL = IMAGE_RESULT_BASE + 9,<br>IMAGE_RESULT_TOO_LARGE = IMAGE_RESULT_BASE + 10,<br>IMAGE_RESULT_TRANSFORM = IMAGE_RESULT_BASE + 11,<br>IMAGE_RESULT_COLOR_CONVERT = IMAGE_RESULT_BASE + 12,<br>IMAGE_RESULT_CROP = IMAGE_RESULT_BASE + 13,<br>IMAGE_RESULT_SOURCE_DATA = IMAGE_RESULT_BASE + 14,<br>IMAGE_RESULT_SOURCE_DATA_INCOMPLETE = IMAGE_RESULT_BASE + 15,<br>IMAGE_RESULT_MISMATCHED_FORMAT = IMAGE_RESULT_BASE + 16,<br>IMAGE_RESULT_UNKNOWN_FORMAT = IMAGE_RESULT_BASE + 17,<br>IMAGE_RESULT_SOURCE_UNRESOLVED = IMAGE_RESULT_BASE + 18,<br>IMAGE_RESULT_INVALID_PARAMETER = IMAGE_RESULT_BASE + 19,<br>IMAGE_RESULT_DECODE_FAILED = IMAGE_RESULT_BASE + 20,<br>IMAGE_RESULT_PLUGIN_REGISTER_FAILED = IMAGE_RESULT_BASE + 21,<br>IMAGE_RESULT_PLUGIN_CREATE_FAILED = IMAGE_RESULT_BASE + 22,<br>IMAGE_RESULT_ENCODE_FAILED = IMAGE_RESULT_BASE + 23,<br>IMAGE_RESULT_ADD_PIXEL_MAP_FAILED = IMAGE_RESULT_BASE + 24,<br>IMAGE_RESULT_HW_DECODE_UNSUPPORT = IMAGE_RESULT_BASE + 25,<br>IMAGE_RESULT_DECODE_HEAD_ABNORMAL = IMAGE_RESULT_BASE + 26,<br>IMAGE_RESULT_DECODE_EXIF_UNSUPPORT = IMAGE_RESULT_BASE + 27,<br>IMAGE_RESULT_PROPERTY_NOT_EXIST = IMAGE_RESULT_BASE + 28,<br>IMAGE_RESULT_MEDIA_DATA_UNSUPPORT = IMAGE_RESULT_BASE + 30,<br>IMAGE_RESULT_MEDIA_TOO_LARGE = IMAGE_RESULT_BASE + 31,<br>IMAGE_RESULT_MEDIA_MALLOC_FAILED = IMAGE_RESULT_BASE + 32,<br>IMAGE_RESULT_MEDIA_END_OF_STREAM = IMAGE_RESULT_BASE + 33,<br>IMAGE_RESULT_MEDIA_IO_ABNORMAL = IMAGE_RESULT_BASE + 34,<br>IMAGE_RESULT_MEDIA_MALFORMED = IMAGE_RESULT_BASE + 35,<br>IMAGE_RESULT_MEDIA_BUFFER_TOO_SMALL = IMAGE_RESULT_BASE + 36,<br>IMAGE_RESULT_MEDIA_OUT_OF_RANGE = IMAGE_RESULT_BASE + 37,<br>IMAGE_RESULT_MEDIA_STATUS_ABNORMAL = IMAGE_RESULT_BASE + 38,<br>IMAGE_RESULT_MEDIA_VALUE_INVALID = IMAGE_RESULT_BASE + 39,<br>IMAGE_RESULT_MEDIA_NULL_POINTER = IMAGE_RESULT_BASE + 40,<br>IMAGE_RESULT_MEDIA_INVALID_OPERATION = IMAGE_RESULT_BASE + 41,<br>IMAGE_RESULT_MEDIA_ERR_PLAYER_NOT_INIT = IMAGE_RESULT_BASE + 42,<br>IMAGE_RESULT_MEDIA_EARLY_PREPARE = IMAGE_RESULT_BASE + 43,<br>IMAGE_RESULT_MEDIA_SEEK_ERR = IMAGE_RESULT_BASE + 44,<br>IMAGE_RESULT_MEDIA_PERMISSION_DENIED = IMAGE_RESULT_BASE + 45,<br>IMAGE_RESULT_MEDIA_DEAD_OBJECT = IMAGE_RESULT_BASE + 46,<br>IMAGE_RESULT_MEDIA_TIMED_OUT = IMAGE_RESULT_BASE + 47,<br>IMAGE_RESULT_MEDIA_TRACK_NOT_ALL_SUPPORTED = IMAGE_RESULT_BASE + 48,<br>IMAGE_RESULT_MEDIA_ADAPTER_INIT_FAILED = IMAGE_RESULT_BASE + 49,<br>IMAGE_RESULT_MEDIA_WRITE_PARCEL_FAIL = IMAGE_RESULT_BASE + 50,<br>IMAGE_RESULT_MEDIA_READ_PARCEL_FAIL = IMAGE_RESULT_BASE + 51,<br>IMAGE_RESULT_MEDIA_NO_AVAIL_BUFFER = IMAGE_RESULT_BASE + 52,<br>IMAGE_RESULT_MEDIA_INVALID_PARAM = IMAGE_RESULT_BASE + 53,<br>IMAGE_RESULT_MEDIA_CODEC_ADAPTER_NOT_EXIST = IMAGE_RESULT_BASE + 54,<br>IMAGE_RESULT_MEDIA_CREATE_CODEC_ADAPTER_FAILED = IMAGE_RESULT_BASE + 55,<br>IMAGE_RESULT_MEDIA_CODEC_ADAPTER_NOT_INIT = IMAGE_RESULT_BASE + 56,<br>IMAGE_RESULT_MEDIA_ZCODEC_CREATE_FAILED = IMAGE_RESULT_BASE + 57,<br>IMAGE_RESULT_MEDIA_ZCODEC_NOT_EXIST = IMAGE_RESULT_BASE + 58,<br>IMAGE_RESULT_MEDIA_JNI_CLASS_NOT_EXIST = IMAGE_RESULT_BASE + 59,<br>IMAGE_RESULT_MEDIA_JNI_METHOD_NOT_EXIST = IMAGE_RESULT_BASE + 60,<br>IMAGE_RESULT_MEDIA_JNI_NEW_OBJ_FAILED = IMAGE_RESULT_BASE + 61,<br>IMAGE_RESULT_MEDIA_JNI_COMMON_ERROR = IMAGE_RESULT_BASE + 62,<br>IMAGE_RESULT_MEDIA_DISTRIBUTE_NOT_SUPPORT = IMAGE_RESULT_BASE + 63,<br>IMAGE_RESULT_MEDIA_SOURCE_NOT_SET = IMAGE_RESULT_BASE + 64,<br>IMAGE_RESULT_MEDIA_RTSP_ADAPTER_NOT_INIT = IMAGE_RESULT_BASE + 65,<br>IMAGE_RESULT_MEDIA_RTSP_ADAPTER_NOT_EXIST = IMAGE_RESULT_BASE + 66,<br>IMAGE_RESULT_MEDIA_RTSP_SURFACE_UNSUPPORT = IMAGE_RESULT_BASE + 67,<br>IMAGE_RESULT_MEDIA_RTSP_CAPTURE_NOT_INIT = IMAGE_RESULT_BASE + 68,<br>IMAGE_RESULT_MEDIA_RTSP_SOURCE_URL_INVALID = IMAGE_RESULT_BASE + 69,<br>IMAGE_RESULT_MEDIA_RTSP_VIDEO_TRACK_NOT_FOUND = IMAGE_RESULT_BASE + 70,<br>IMAGE_RESULT_MEDIA_RTSP_CAMERA_NUM_REACH_MAX = IMAGE_RESULT_BASE + 71,<br>IMAGE_RESULT_MEDIA_SET_VOLUME = IMAGE_RESULT_BASE + 72,<br>IMAGE_RESULT_MEDIA_NUMBER_OVERFLOW = IMAGE_RESULT_BASE + 73,<br>IMAGE_RESULT_MEDIA_DIS_PLAYER_UNSUPPORTED = IMAGE_RESULT_BASE + 74,<br>IMAGE_RESULT_MEDIA_DENCODE_ICC_FAILED = IMAGE_RESULT_BASE + 75,<br>IMAGE_RESULT_MEDIA_ENCODE_ICC_FAILED = IMAGE_RESULT_BASE + 76,<br>IMAGE_RESULT_MEDIA_READ_PIXELMAP_FAILED = IMAGE_RESULT_BASE + 150,<br>IMAGE_RESULT_MEDIA_WRITE_PIXELMAP_FAILED = IMAGE_RESULT_BASE + 151,<br>IMAGE_RESULT_MEDIA_PIXELMAP_NOT_ALLOW_MODIFY = IMAGE_RESULT_BASE + 152,<br>IMAGE_RESULT_MEDIA_CONFIG_FAILED = IMAGE_RESULT_BASE + 153,<br>IMAGE_RESULT_JNI_ENV_ABNORMAL = IMAGE_RESULT_BASE + 154,<br>IMAGE_RESULT_SURFACE_GRALLOC_BUFFER_FAILED = IMAGE_RESULT_BASE + 155,<br>IMAGE_RESULT_CREATE_SURFACE_FAILED = IMAGE_RESULT_BASE + 156,<br>IMAGE_RESULT_SURFACE_GET_PARAMETER_FAILED = IMAGE_RESULT_BASE + 157,<br>IMAGE_RESULT_GET_SURFACE_FAILED = IMAGE_RESULT_BASE + 158,<br>IMAGE_RESULT_SURFACE_ACQUIRE_BUFFER_FAILED = IMAGE_RESULT_BASE + 159,<br>IMAGE_RESULT_SURFACE_REQUEST_BUFFER_FAILED = IMAGE_RESULT_BASE + 160,<br>IMAGE_RESULT_REGISTER_LISTENER_FAILED = IMAGE_RESULT_BASE + 161,<br>IMAGE_RESULT_REGISTER_BUFFER_FAILED = IMAGE_RESULT_BASE + 162,<br>IMAGE_RESULT_FREAD_FAILED = IMAGE_RESULT_BASE + 163,<br>IMAGE_RESULT_PEEK_FAILED = IMAGE_RESULT_BASE + 164,<br>IMAGE_RESULT_SEEK_FAILED = IMAGE_RESULT_BASE + 165,<br>IMAGE_RESULT_STREAM_SIZE_ERROR = IMAGE_RESULT_BASE + 166,<br>IMAGE_RESULT_FILE_FD_ERROR = IMAGE_RESULT_BASE + 167,<br>IMAGE_RESULT_FILE_DAMAGED = IMAGE_RESULT_BASE + 168,<br>IMAGE_RESULT_CREATE_DECODER_FAILED = IMAGE_RESULT_BASE + 169,<br>IMAGE_RESULT_CREATE_ENCODER_FAILED = IMAGE_RESULT_BASE + 170,<br>IMAGE_RESULT_CHECK_FORMAT_ERROR = IMAGE_RESULT_BASE + 171,<br>IMAGE_RESULT_THIRDPART_SKIA_ERROR = IMAGE_RESULT_BASE + 172,<br>IMAGE_RESULT_HW_DECODE_FAILED = IMAGE_RESULT_BASE + 173,<br>IMAGE_RESULT_ALLOCATER_TYPE_ERROR = IMAGE_RESULT_BASE + 174,<br>IMAGE_RESULT_ALPHA_TYPE_ERROR = IMAGE_RESULT_BASE + 175,<br>IMAGE_RESULT_INDEX_INVALID = IMAGE_RESULT_BASE + 176,<br>IMAGE_RESULT_MEDIA_UNKNOWN = IMAGE_RESULT_BASE + 200<br>} | Enumerates the return values that may be used by the interface.| 
