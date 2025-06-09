/**
 * 最佳实践: 音乐播放场景低功耗规则
 */

// [Start setAudioEffectMode]
// [Start audioStreamInfo]
import { audio } from '@kit.AudioKit';
// [End audioStreamInfo]
import {BusinessError} from '@kit.BasicServicesKit';
// [End setAudioEffectMode]
// [Start setListener]
import { avSession as AVSessionManager } from '@kit.AVSessionKit';
// [End setListener]

let   audioRenderer: audio.AudioRenderer | undefined = undefined;
// [Start setAudioEffectMode]
audioRenderer!.setAudioEffectMode(audio.AudioEffectMode.EFFECT_NONE, (err: BusinessError) => {
  if (err) {
    console.error(`Failed to set params, code is ${err.code}, message is ${err.message}`);
    return;
  } else {
    console.info('Callback invoked to indicate a successful audio effect mode setting.');
  }
});
// [End setAudioEffectMode]

// [Start audioStreamInfo]
let audioStreamInfo: audio.AudioStreamInfo = {
  samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
  channels: audio.AudioChannel.CHANNEL_1,
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
audio.createAudioRenderer(audioRendererOptions, (err, data) => {
  if (err) {
    console.error(`Invoke createAudioRenderer failed, code is ${err.code}, message is ${err.message}`);
    return;
  } else {
    console.info('Invoke createAudioRenderer succeeded.');
    let audioRenderer = data;
  }
});
// [End audioStreamInfo]

// [Start setListener]
const uiContext: UIContext | undefined = AppStorage.get('uiContext');
let context: Context = uiContext?.getHostContext()!;
async function setListener() {
  // Assuming that a session has been created, see the previous example for how to create a session
  let type: AVSessionManager.AVSessionType = 'audio';
  let session = await AVSessionManager.createAVSession(context, 'SESSION_NAME', type);

  // Set the duration of the property
  let metadata: AVSessionManager.AVMetadata = {
    assetId: '0',
    title: 'TITLE',
    mediaImage: 'IMAGE',
    duration: 23000, // The duration of the resource, measured in milliseconds
  };
  session.setAVMetadata(metadata).then(() => {
    console.info(`SetAVMetadata successfully`);
  }).catch((err: BusinessError) => {
    console.error(`Failed to set AVMetadata. Code: ${err.code}, message: ${err.message}`);
  });

  // Set Status: Playback Status, Progress Position, Playback Speed, Cache Time
  let playbackState: AVSessionManager.AVPlaybackState = {
    state: AVSessionManager.PlaybackState.PLAYBACK_STATE_PLAY, // Playback status
    position: {
      elapsedTime: 1000, // The position that has been played, in ms
      updateTime: new Date().getTime(), // The timestamp of when the app updated the current location, in ms
    },
    speed: 1.0, // Optional, the default is 1.0, the speed of playback, set according to the speed supported in the app, the system does not do verification
    bufferedTime: 14000, // Optional, the time for which the resource is cached, in ms
  };
  session.setAVPlaybackState(playbackState, (err) => {
    if (err) {
      console.error(`Failed to set AVPlaybackState. Code: ${err.code}, message: ${err.message}`);
    } else {
      console.info(`SetAVPlaybackState successfully`);
    }
  });
}
// [End setListener]