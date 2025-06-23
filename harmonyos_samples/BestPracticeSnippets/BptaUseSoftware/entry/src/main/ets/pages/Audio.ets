/**
 * 最佳实践：合理使用音频播放
 */

// [Start audio_bpta]
import { fileIo as fs } from '@kit.CoreFileKit';
// [StartExclude audio_bpta]
import { audio } from '@kit.AudioKit';

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
// [EndExclude audio_bpta]

const uiContext: UIContext | undefined = AppStorage.get('uiContext');
let context = uiContext!.getHostContext()!;

async function read() {
  const bufferSize: number = await audioRenderer.getBufferSize();
  let path = context.filesDir; // Path of the file

  const filePath = path + '/voice_call_data.wav'; // Prohibit the file from being played silently
  let file: fs.File = fs.openSync(filePath, fs.OpenMode.READ_ONLY); // Open the file
  let buf = new ArrayBuffer(bufferSize);
  let readSize: number = await fs.read(file.fd, buf); // Read the file content
  let writeSize: number = await audioRenderer.write(buf); // Play the file content
}
// [End audio_bpta]