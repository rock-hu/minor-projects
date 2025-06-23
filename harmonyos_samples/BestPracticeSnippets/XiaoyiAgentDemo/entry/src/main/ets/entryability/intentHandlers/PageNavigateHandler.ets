// [Start router]
import { insightIntent } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';

export default class PageNavigateHandler {
  async execute(): Promise<insightIntent.ExecuteResult> {
    try {
      // Execute page redirection.
      let uiContext: UIContext | null | undefined = null;
      uiContext = AppStorage.get('uiContext')
      uiContext?.getRouter().pushUrl({ url: 'pages/SecondPage' });

      return {
        code: 0,
        result: {
          status: 'success',
          message: 'Navigation successful',
          targetPage: 'SecondPage'
        }
      };
    } catch (error) {
      throw new Error(`Page navigation failed: ${(error as BusinessError).message}`);
    }
  }
}
// [End router]