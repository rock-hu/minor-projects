/**
 * 最佳实践：后台合理使用上传下载
 * 场景二：下载
 */

// [Start software_download]
import { BusinessError, request } from '@kit.BasicServicesKit';

try {
  request.downloadFile(this.getUIContext().getHostContext(), {
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