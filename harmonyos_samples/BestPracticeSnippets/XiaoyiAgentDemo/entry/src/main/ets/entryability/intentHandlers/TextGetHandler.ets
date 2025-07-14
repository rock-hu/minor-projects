// [Start name]
import { insightIntent } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { MediaService } from '../../utils/audioplayer/MediaService';

export default class TextGetHandler {
  async execute(): Promise<insightIntent.ExecuteResult> {
    try {
      // Execute the method of obtaining the name of the played audio.
      const audioName: string = MediaService.getInstance().getAudioFileName();

      // Return result.
      return {
        code: 0,
        result: {
          status:'success',
          audioName: audioName,
          message: 'get audio name success'
        }
      };
    } catch (error) {
      throw new Error(`get audio name failed: ${(error as BusinessError).message}`);
    }
  }
}
// [End name]