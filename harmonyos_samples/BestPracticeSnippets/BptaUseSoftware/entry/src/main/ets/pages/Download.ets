/**
 * 最佳实践：后台合理使用上传下载
 * 场景二：下载
 */
// [Start software_download]
import { BusinessError, request } from '@kit.BasicServicesKit';
import { common } from '@kit.AbilityKit';

// [End software_download]

@Entry
@Component
struct Download {
  build() {
  }


  funcA() {
    // [Start software_download]
    try {
      let context = this.getUIContext().getHostContext() as common.UIAbilityContext;
      request.downloadFile(context, {
        url: 'https://xxxx/xxxxx.hap', // IP address of the server to download the file
        filePath: 'xxx/xxxxx.hap'
      }, (err: BusinessError, data: request.DownloadTask) => {
        if (err) {
          console.error(`Failedtorequestthedownload.Code:${err.code},message:${err.message}`);
          return;
        }
        let downloadTask: request.DownloadTask = data;
      });
    } catch (err) {
      console.error(`Failedtorequestthedownload.err:${JSON.stringify(err)}`);
    }
    // [End software_download]
  }
}


