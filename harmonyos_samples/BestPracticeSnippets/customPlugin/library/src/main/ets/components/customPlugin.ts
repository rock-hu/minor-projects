/*
* Copyright (C) 2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
//[Start custom_plugin]
import { fileIo as fs } from '@kit.CoreFileKit';
import { util } from '@kit.ArkTS';
/**
 *
 *最佳实践: 定制hvigor插件开发实践
 */
interface OhPackage {
  name: string;
  version: number;
  description: string;
  author: string;
}
//[Start readFileSync]
function readFileSync(filePath: string): string {
  try {
    const fd = fs.openSync(filePath, fs.OpenMode.READ_ONLY).fd;
    const stat = fs.statSync(filePath);

    const buffer = new ArrayBuffer(stat.size);
    const bufferView = new Uint8Array(buffer);

    const bytesRead = fs.readSync(
      fd,
      bufferView,
      { length: stat.size }
    );

    fs.closeSync(fd);

    const textDecoder = new util.TextDecoder('utf-8');
    return textDecoder.decodeWithStream(bufferView);
  } catch (err) {
    console.error('Read file failed:', err);
    throw err; // 重新抛出错误以便上层处理
  }
}
//[End readFileSync]

//[Start renameHarTask]
export function renameHarTask(str?: string) {
  return {
    pluginId: 'RenameHarTaskID',
    apply(pluginContext) {
      pluginContext.registerTask({
        // 编写自定义任务
        name: 'renameHarTask',
        run: (taskContext) => {
          // 读取oh-package.json5，解析出version
          const packageFile = taskContext.modulePath+'\\oh-package.json5';
          console.log('file: ', packageFile);
          let fileContent = this.readFileSync(packageFile, 'utf8');
          console.log(fileContent);
          const content: OhPackage = JSON.parse(fileContent);
          const version = content.version;
          const author = content.author;


          console.log('renameHarTask: ', taskContext.moduleName, taskContext.modulePath);
          const sourceFile = taskContext.modulePath + '\\build\\default\\outputs\\default\\' + taskContext.moduleName + '.har';
          const targetPath = taskContext.modulePath + '\\build\\default\\outputs\\target\\';
          const targetFile = targetPath
            + taskContext.moduleName + '-' + version + '-' + author +'.har';
          console.log('renameHarTask: sourceFile: ', sourceFile);
          console.log('renameHarTask: targetFile: ', targetFile);


          // 创建目录
          fs.mkdir(targetPath, true, (err) => {
            if (err) throw err;
            // 移动并修改产物文件名
            fs.rename(sourceFile, targetFile, (err)=> {
              console.log('err: ' + err);
            });
          });
        },
        // 确认自定义任务插入位置
        dependencies: ['default@PackageHar'],
        postDependencies: ['assembleHar']
      })
    }
  }
  //[End renameHarTask]
}
//[End custom_plugin]