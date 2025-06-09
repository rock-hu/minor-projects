
/**
 * 视频场景编解码低功耗规则
 */


// [Start VideoSceneEncoding]
// To create a decoder using a codec name, if the application has special requirements, such as selecting a decoder that supports a certain resolution specification, 
// the capability can be queried first, and then the decoder can be created based on the codec name.
OH_AVCapability *capability = OH_AVCodec_GetCapability(OH_AVCODEC_MIMETYPE_VIDEO_AVC, false);
const char *name = OH_AVCapability_GetName(capability);
// Create a decoder through mimetype
// Hard solution: Create H264 decoder. When there are multiple optional decoders, the system will create the most suitable decoder
OH_AVCodec *videoDec = OH_VideoDecoder_CreateByMime(OH_AVCODEC_MIMETYPE_VIDEO_AVC);
// Hard solution: Create H265 decoder
OH_AVCodec *videoDec = OH_VideoDecoder_CreateByMime(OH_AVCODEC_MIMETYPE_VIDEO_HEVC);
// [End VideoSceneEncoding]