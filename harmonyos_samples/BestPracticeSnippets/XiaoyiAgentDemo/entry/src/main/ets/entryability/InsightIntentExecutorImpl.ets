// [Start executorImpl]
import { insightIntent, InsightIntentExecutor } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import AudioPlayHandler from './intentHandlers/AudioPlayHandler';
import TextGetHandler from './intentHandlers/TextGetHandler';
import PageNavigateHandler from './intentHandlers/PageNavigateHandler';
import { window } from '@kit.ArkUI';

export default class InsightIntentExecutorImpl extends InsightIntentExecutor {
  // Instruction implementation class.
  private audioHandler = new AudioPlayHandler();
  private textHandler = new TextGetHandler();
  private pageHandler = new PageNavigateHandler();

  // Intention execution method for backend execution.
  async onExecuteInUIAbilityBackgroundMode(
    intentName: string,
    params: Record<string, object>
  ): Promise<insightIntent.ExecuteResult> {

    try {
      switch (intentName) {
        // Play audio intention.
        case 'PlayHiddenAudio':
          const stringParam: Record<string, string> = this.convertToRecord(params);
          return this.audioHandler.execute(stringParam);

        // Obtain audio name intent.
        case 'GetAudioName':
          return this.textHandler.execute();

        default:
          return {
            code: -1,
            result: {
              'status': 'failed',
              'message': `not valid intent name, ${intentName}`
            }
          };
      }
    } catch (error) {
      return {
        code: -2,
        result: {
          'status': 'failed',
          'message': `Intent execution failed: ${(error as BusinessError).message}`
        }
      };
    }
  }

  private convertToRecord(origin: Record<string, object>): Record<string, string> {
    const result: Record<string, string> = {};
    Object.keys(origin).forEach(key => {
      result[key] = String(origin[key]);
    });

    return result;
  }

  // Execution method of backend execution intention.
  async onExecuteInUIAbilityForegroundMode(name: string, param: Record<string, Object>, pageLoader: window.WindowStage):
    Promise<insightIntent.ExecuteResult> {
    switch (name) {
      // Open the second page intention.
      case 'OpenSecondPage':
        return this.pageHandler.execute();
      default:
        pageLoader.loadContent('pages/MainPage');
        break;
    }
    return Promise.resolve({
      code: -1,
      result: {
        message: 'unknown intent'
      }
    } as insightIntent.ExecuteResult)
  }
}
// [End executorImpl]