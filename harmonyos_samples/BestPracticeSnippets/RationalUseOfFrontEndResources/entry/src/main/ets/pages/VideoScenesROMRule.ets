/**
 * 最佳实践: 视频场景ROM低功耗建议
 */

import { fileIo as fileIo } from '@kit.CoreFileKit';
import { BusinessError } from '@kit.BasicServicesKit';

const pathDir = ""
// [Start filePath]
let filePath = pathDir + "/test.txt";
let file = fileIo.openSync(filePath, fileIo.OpenMode.READ_WRITE | fileIo.OpenMode.CREATE);
let str: string = "hello, world";
// Use asynchronous methods to write files to the ROM
fileIo.write(file.fd, str).then((writeLen: number) => {
  console.info("write data to file succeed and size is:" + writeLen);
}).catch((err: BusinessError) => {
  console.error("write data to file failed with error message: " + err.message + ", error code: " + err.code);
}).finally(() => {
  fileIo.closeSync(file);
});
// [End filePath]


