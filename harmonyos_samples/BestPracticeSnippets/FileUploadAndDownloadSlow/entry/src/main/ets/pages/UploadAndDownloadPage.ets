// [Start create_config]
let config: request.agent.Config = {
  action: request.agent.Action.DOWNLOAD, // Configure task options, which are configured as download tasks.
  url: downloadUrl, // Configure the download task url
  overwrite: true, // Solution selection when the path already exists during the download process. True means overwriting existing files.
  method: 'GET', // HTTP standard method. When downloading, use GET or POST
  saveas: './', // Here'. /' indicates that the download is downloaded to the current cache path of the application.
  mode: request.agent.Mode.BACKGROUND, // Task mode setting background tasks
  gauge: true // The process progress notification strategy of background tasks is only applicable to background tasks. True means to send a notification that each progress has been completed or failed.
};
// [End create_config]

@Component
struct UploadAndDownloadPage {

  create() {
    // [Start request_agent_create]
    request.agent.create(context, this.downloadConfig).then((task: request.agent.Task) => {
      // Callbacks related to the registration download task
      task.on('completed', this.completedCallback); // Complete the callback
      task.on('failed', this.failedCallback); // Failed callback
      task.on('pause', this.pauseCallback); // Pause the callback
      task.on('resume', this.resumeCallback); // Restart the callback
      task.on('progress', this.progressCallback); // Progress update callback
      task.on('response', this.progressCallback); // Response header data callback
    }).catch((err: BusinessError) => {
      logger.error(TAG, `Failed to task create with error message: ${err.message}, error code: ${err.code}`);
    });
    // [End request_agent_create]
  }

  start() {
    // [Start task_start]
    task.start().then(() => {
      this.downloadTask = task;
    }).catch((err: Error) => {
      logger.error(TAG, 'task start error:', err);
    })
    // [End task_start]
  }

  // [Start file_download_item]
  FileDownloadItem() {
    // Determine whether the current download task status meets the pause conditions
    if (this.downloadTask && (taskInfo.progress.state === request.agent.State.WAITING || taskInfo.progress.state
      === request.agent.State.RUNNING || taskInfo.progress.state === request.agent.State.RETRYING)) {
      this.downloadTask.pause().then(() => {
      }).catch((err: Error) => {
        logger.error(TAG, 'task pause error:', err);
      });
    }

    // Determine if the task is suspended, restart the download task.
    if (this.downloadTask && taskInfo.progress.state === request.agent.State.PAUSED) {
      this.downloadTask.resume().then(() => {
      }).catch((err: Error) => {
        logger.error(TAG, 'task resume error:', err);
      });
    }
  }
  // [End file_download_item]


  // [Start request_agent_create]
  private completedCallback = (progress: request.agent.Progress) => {
    // The download status is set to download completion.
    this.state = "下载完成";
    if (this.sFileSize === '未知大小') {
      this.nCurrentDownloadSize = 1;
    }
  // The file download is completed, and the number of tasks to be downloaded will be reduced by 1
    this.downloadCount--;
    this.isShow = false;
  }
  // [End request_agent_create]

  build() {
    Column() {
      // [Start download_config_array]
      ForEach(this.downloadConfigArray, (item: request.agent.Config) => {
        ListItem() {
          // Create a file download monitoring instance
          FileDownloadItem({
            downloadConfig: item, // File download configuration
            isStartAllDownload: this.isStartAllDownload, // Do you start downloading all?
            downloadCount: this.downloadCount // Number of tasks to be downloaded
          })
        }
      }, (item: request.agent.Config) => JSON.stringify(item))
      // [End download_config_array]
    }
  }
}