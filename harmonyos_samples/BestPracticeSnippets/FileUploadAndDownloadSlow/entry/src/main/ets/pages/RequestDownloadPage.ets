// [Start import_request_download]
import { common } from '@kit.AbilityKit';
import { request } from '@kit.BasicServicesKit';
// [End import_request_download]

// [Start request_download]
class RequestDownload {
  // Uri before the task is stored
  private waitList: Array<string[]> = [];
  // Uri before the task is stored
  private downloadTask: request.agent.Task | undefined = undefined;
  // ...
}
// [End request_download]

@Component
struct RequestDownloadPage {
  // [Start request_download_page]
  async createBackgroundTask(downloadList: Array<string[]>) {
    if (this.context === undefined) {
      return;
    }
    for (let i = 0; i < downloadList.length; i++) {
      try {
        let splitUrl = downloadList[i][1].split('//')[1].split('/');
        let downloadConfig: request.agent.Config = {
          action: request.agent.Action.DOWNLOAD,
          url: downloadList[i][1],
          method: 'POST',
          title: 'download',
          mode: request.agent.Mode.BACKGROUND, // It must be a background task to continue.
          network: request.agent.Network.ANY,
          saveas: `./${downloadList[i][0]}/${splitUrl[splitUrl.length-1]}`,
          overwrite: true,
          gauge: true
        }
        let downTask = await request.agent.create(this.context, downloadConfig);
        // [Start down_task_start]
        // [StartExclude down_task_start]
        // [EndExclude down_task_start]
        await downTask.start();
        // [StartExclude down_task_start]
        // [EndExclude down_task_start]
        // [End down_task_start]
      } catch (err) {
        logger.error(TAG, `task  err, err  = ${JSON.stringify(err)}`);
        this.waitList.push(downloadList[i]);
      }
    }
  }
    // [End request_download_page]
  build() {

  }
}