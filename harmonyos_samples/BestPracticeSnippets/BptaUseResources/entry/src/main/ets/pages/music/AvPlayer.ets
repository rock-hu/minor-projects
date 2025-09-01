/**
 * 最佳实践：合理使用音频资源
 * 场景二：播音场景（AVPlayer）
 */
// [Start sound_pool]
import { fileIo as fs } from '@kit.CoreFileKit';
import { media } from '@kit.MediaKit';

// [End sound_pool]

// [Start av_player]
import { UIAbility } from '@kit.AbilityKit';

// [StartExclude av_player]
let avPlayer: media.AVPlayer
// [EndExclude av_player]

export default class EntryAbility extends UIAbility {
  // ...
  onForeground(): void {
    //Playing according to service requirements in the foreground
    avPlayer.play();
  }

  onBackground(): void {
    // Return to the background to stop playing or pause
    avPlayer.stop(); // Or pause();
  }
}
// [End av_player]

/**
 * 最佳实践：合理使用音频资源
 * 场景五：播音场景（SoundPool）
 */

let soundId: number;
let streamId: number;
let uri: string;
let soundPool: media.SoundPool;
// [Start sound_pool]
const SoundPool = async () => {

  //Construct the audio stream to play
  await fs.open('/test_01.mp3', fs.OpenMode.READ_ONLY).then((file: fs.File) => {
    console.info("filefd:" + file.fd);
    uri = 'fd://' + (file.fd).toString()
  }); // '/test_01.mp3' is used as an example. The path of the file needs to be transferred
  soundId = await soundPool.load(uri);
  //The foreground scene starts to play
  streamId = await soundPool.play(soundId);
  //Stop playing in the background scenario: soundPool.stop (streamId);
}
// [End sound_pool]
