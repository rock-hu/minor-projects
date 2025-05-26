// [Start import_request]
import { common } from '@kit.AbilityKit';
import { request } from '@kit.BasicServicesKit';
// [End import_request]

// [Start class_upload]
class Upload {
  // Background tasks
  private backgroundTask: request.agent.Task | undefined = undefined;
  // The uri stored before creating the task
  private waitList: Array<string> = [];
  // ...
}
// [End class_upload]

@Component
struct FileUploadPage {
  // [Start file_upload_page]
  private config: request.agent.Config = {
    action: request.agent.Action.UPLOAD,
    headers: HEADER,
    url: '',
    mode: request.agent.Mode.BACKGROUND,
    method: 'POST',
    title: 'upload',
    network: request.agent.Network.ANY,
    data: [],
    token: 'UPLOAD_TOKEN'
  }
  // ...
  // Convert uri
  private async getFilesAndData(cacheDir: string, fileUris: Array<string>): Promise<Array<request.agent.FormItem>> {
    // ...
  }
  // Create a file upload background task
  async createBackgroundTask(fileUris: Array<string>) {
    if (this.context === undefined) {
      return;
    }
    // Get the upload URL
    this.config.url = await urlUtils.getUrl(this.context);
    this.config.data = await this.getFilesAndData(this.context.cacheDir, fileUris);
    this.config.mode = request.agent.Mode.BACKGROUND;
    try {
      this.backgroundTask = await request.agent.create(this.context, this.config);
      // [Start background_task_start]
      await this.backgroundTask.start();
      // [End background_task_start]
      let state = AppStorage.get<number>('backTaskState');
      if (state === BackgroundTaskState.PAUSE) {
        await this.backgroundTask.pause();
      }
      logger.info(TAG, `createBackgroundTask success`);
    } catch (err) {
      logger.error(TAG, `task  err, err  = ${JSON.stringify(err)}`);
    }
  }
  // [End file_upload_page]

  // [Start background_task_pause]
  async pause() {
    // ...
    if (this.backgroundTask === undefined) {
      return;
    }
    await this.backgroundTask.pause();
  }
  // [End background_task_pause]

  // [Start background_task_resume]
  async resume() {
    // ...
    if (this.backgroundTask === undefined) {
      return;
    }
    await this.backgroundTask.resume();
  }
  // [End background_task_resume]

  build() {

  }
}