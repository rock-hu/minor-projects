/**
 * 最佳实践：后台合理使用系统资源
 * 场景一：系统帮助应用持锁
 */

// [Start bpta_lock_by_system]
import { UIAbility } from '@kit.AbilityKit';
import { audio } from '@kit.AudioKit';
import { BusinessError } from '@kit.BasicServicesKit';
// [StartExclude bpta_lock_by_system]
let audioStreamInfo: audio.AudioStreamInfo = {
  samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_48000,
  channels: audio.AudioChannel.CHANNEL_2,
  sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
  encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW
};

let audioRendererInfo: audio.AudioRendererInfo = {
  usage: audio.StreamUsage.STREAM_USAGE_MUSIC,
  rendererFlags: 0
};

let audioRendererOptions: audio.AudioRendererOptions = {
  streamInfo: audioStreamInfo,
  rendererInfo: audioRendererInfo
};
let audioRenderer: audio.AudioRenderer;
audio.createAudioRenderer(audioRendererOptions, (err, data) => {
  if (err) {
    console.error(`Invoke createAudioRenderer failed, code is ${err.code}, message is ${err.message}`);
    return;
  } else {
    console.info('Invoke createAudioRenderer succeeded.');
    let audioRenderer = data;
  }
});
// [EndExclude bpta_lock_by_system]
export default class EntryAbility extends UIAbility {
  // ...

  onForeground(): void {
    //Apply for the resources required by the system, or reapply for the resources released in onBackground ()
    audio.createAudioRenderer(audioRendererOptions,(err: BusinessError) => {});
  }

  onBackground(): void {
    //Release resources when the UI is invisible
    audioRenderer.stop((err: BusinessError) => {});
  }
}
// [End bpta_lock_by_system]