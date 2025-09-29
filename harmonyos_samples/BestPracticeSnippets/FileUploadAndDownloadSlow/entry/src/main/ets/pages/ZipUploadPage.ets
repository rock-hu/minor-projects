// [Start import_zlib]
import { common } from '@kit.AbilityKit';
import { fileIo } from '@kit.CoreFileKit';
import { zlib } from '@kit.BasicServicesKit';
// [End import_zlib]

// [Start zip_upload]
class ZipUpload {
  // Uri stored before creating the task
  private waitList: Array<string> = [];
  // Files that need to be uploaded uri
  private fileUris: Array<string> = [];
  // ...
}
// [End zip_upload]

@Component
struct ZipUploadPage {
  private context: common.UIAbilityContext | undefined = undefined;
  private waitList: Array<string> = [];

  // [Start zip_upload_page]
  // Data compression processing
  async zipUploadFiles(fileUris: Array<string>): Promise<void> {
    this.context = this.getUIContext().getHostContext() as common.UIAbilityContext;
    let cacheDir = this.context.cacheDir;
    let tempDir = fileIo.mkdtempSync(`${cacheDir}/XXXXXX`);
    // Put the uri obtained from the library picture into fileUris and copy it to the temporary folder.
    for (let i = 0; i < fileUris.length; i++) {
      let fileName = fileUris[i].split('/').pop();
      let resourceFile: fileIo.File = fileIo.openSync(fileUris[i], fileIo.OpenMode.READ_ONLY);
      fileIo.copyFileSync(resourceFile.fd, `${tempDir}/${fileName}`, 0);
      fileIo.closeSync(resourceFile);
    }
    // File compression, package the previously generated temporary folder into test.zip
    let options: zlib.Options = {
      level: zlib.CompressLevel.COMPRESS_LEVEL_DEFAULT_COMPRESSION,
      memLevel: zlib.MemLevel.MEM_LEVEL_DEFAULT,
      strategy: zlib.CompressStrategy.COMPRESS_STRATEGY_DEFAULT_STRATEGY
    };
    let data = await zlib.compressFile(tempDir, `${cacheDir}/test.zip`, options);
    // Delete temporary folders
    fileIo.rmdirSync(tempDir);
    // Put the generated zip package into the transmission queue.
    this.waitList.push(`${cacheDir}/test.zip`);
  }
  // [End zip_upload_page]
  build() {

  }
}