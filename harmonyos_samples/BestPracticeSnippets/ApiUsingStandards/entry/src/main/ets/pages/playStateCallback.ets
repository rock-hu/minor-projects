/**
 * 易错API的使用规范
 */
import { BusinessError } from '@kit.BasicServicesKit';
import { avSession } from '@kit.AVSessionKit';

const TAG = 'TEST';

class avSessionClass {
  castController!: avSession.AVCastController;
  session!: avSession.AVSession;
  currentState: avSession.AVPlaybackState | undefined = undefined;
  currentTime: number = 1;
  playStateCallback: (state: avSession.AVPlaybackState) => void = () => {
  }

  create(context: Context, tag: string) {
    avSession.createAVSession(context, tag, "audio").then(async (currentAVSession: avSession.AVSession) => {
      this.session = currentAVSession;
      let sessionController = await currentAVSession.getAVCastController();
      this.castController = sessionController;
      // [Start play_state_callback]
      let playStateCallback = (state: avSession.AVPlaybackState) => {

      }
      sessionController.on('playbackStateChange', 'all', this.playStateCallback);
      sessionController.off('playbackStateChange', this.playStateCallback);
      // [End play_state_callback]
    }).catch((err: BusinessError) => {
      console.info(`CreateAVSession BusinessError: code: ${err.code}, message: ${err.message}`);
    });
  }

  prepare() {
    let playItem: avSession.AVQueueItem = {
      itemId: 0,
      description: {
        assetId: '12345',
        mediaType: 'AUDIO',
        mediaUri: 'http://resource1_address',
        mediaSize: 12345,
        startPosition: 0,
        duration: 0,
        artist: 'mysong',
        albumTitle: 'song1_title',
        albumCoverUri: "http://resource1_album_address",
        lyricUri: "http://resource1_lyric_address",
        appName: 'MyMusic'
      }
    };
    // [Start prepare]
    // Preparing to play, this will not trigger actual playback, it will load and buffer
    this.castController?.prepare(playItem, () => {
      console.info('Preparation done');
      // Start playback, truly triggering the playback on the other end. Please call start after Prepare is successful.
      this.castController?.start(playItem, () => {
        console.info('Playback started');
      });
    });
    // [End prepare]
  }

  onPrepare() {
    // [Start on_prepare]
    this.session?.on('pause', async () => {
      console.info(TAG, `on pause , do pause task`);
      // Execute player pause
      // Set the status and progress during pause
      this.currentState = {
        state: avSession.PlaybackState.PLAYBACK_STATE_PAUSE,
        position: {
          elapsedTime: this.currentTime,
          updateTime: new Date().getTime(),
        }
      };
      await this.session?.setAVPlaybackState(this.currentState);
    });
    // [End on_prepare]
  }
}
