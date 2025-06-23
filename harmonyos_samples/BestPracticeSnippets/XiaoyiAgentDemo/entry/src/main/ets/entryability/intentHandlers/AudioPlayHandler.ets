// [Start play]
import { insightIntent } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { MediaService } from '../../utils/audioplayer/MediaService';

export default class AudioPlayHandler {

  async execute(param: Record<string, string>): Promise<insightIntent.ExecuteResult> {
    try {
      // Analyze the parameters passed in by the plugin on the parsing end.
      const rawAudioName = param.audioName;
      const audioName = rawAudioName.toString();

      // The method of playing audio can be customized by developers according to their actual business needs.
      MediaService.getInstance().initAudioPlayer(audioName);

      // Return the result to the end plugin.
      return {
        code: 0,
        result: {
          status: 'success',
          message: `play audio ${audioName} started`,
        }
      };
    } catch (error) {
      throw new Error(`Audio play failed: ${(error as BusinessError).message}`);
    }
  }
}
// [End play]