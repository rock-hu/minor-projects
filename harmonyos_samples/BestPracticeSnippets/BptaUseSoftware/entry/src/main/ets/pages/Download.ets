/**
 * 最佳实践：后台合理使用上传下载
 * 场景二：下载
 */

import { BusinessError, request } from '@kit.BasicServicesKit';

try {
  request.downloadFile(getContext(), {
    url: 'https://xxxx/xxxxx.hap', //需要下载的文件的服务器地址
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