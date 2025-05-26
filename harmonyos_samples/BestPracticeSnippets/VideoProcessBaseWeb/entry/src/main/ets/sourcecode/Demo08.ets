/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { webview } from '@kit.ArkWeb';
import { fileIo } from '@kit.CoreFileKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { Constants } from '../common/Constants';
import { promptAction } from '@kit.ArkUI';
const uiContext: UIContext | undefined = AppStorage.get('uiContext');
let context = uiContext!.getPromptAction()
interface ControllerManager {
  registerController(name: string, controller: webview.WebviewController): void;

  getController(name: string): webview.WebviewController | undefined;
}

abstract class AbstractWebDownloadManager {
  abstract start(controllerName: string, path: string, url: string): void;

  abstract pause(): void;

  abstract resume(): void;

  abstract cancel(): void;
}

@Observed
export class WebDownloadManager extends AbstractWebDownloadManager
implements ControllerManager {
  downloadSpeed: number = 0;
  downloadProgress: number = 0;
  taskStatus: webview.WebDownloadState = webview.WebDownloadState.IN_PROGRESS;
  private static instance: WebDownloadManager;
  private sourcePath: string = '';
  private failedData: Uint8Array = new Uint8Array();
  private controllers: Map<string, webview.WebviewController> = new Map();
  private currentTask: webview.WebDownloadItem = new webview.WebDownloadItem();
  // Define the download delegate instance.
  private delegate: webview.WebDownloadDelegate = new webview.WebDownloadDelegate();

  private constructor() {
    super()
  }

  public static getInstance() {
    if (!WebDownloadManager.instance) {
      WebDownloadManager.instance = new WebDownloadManager();
    }
    return WebDownloadManager.instance;
  }

  public start(controllerName: string, path: string, url: string): Promise<string> {
    return new Promise((resolve, reject) => {
      const controller = this.getController(controllerName);
      // DocsCode 1735041636700
      // src/main/ets/utils/WebDownloadManager.ets

      this.delegate.onBeforeDownload((webDownloadItem: webview.WebDownloadItem) => {
        this.currentTask = webDownloadItem;
        this.sourcePath = path + '/' + webDownloadItem.getSuggestedFileName();
        // Specify the download path before downloading.
        this.currentTask.start(this.sourcePath);
        this.taskStatus = webDownloadItem.getState();
      })
      // DocsCode 1735041636700
      // DocsCode 1735041667172
      // src/main/ets/utils/WebDownloadManager.ets

      this.delegate.onDownloadUpdated((webDownloadItem: webview.WebDownloadItem) => {
        this.currentTask = webDownloadItem;
        // maintain download progress.
        this.downloadProgress = webDownloadItem.getPercentComplete();
        this.downloadSpeed = webDownloadItem.getCurrentSpeed();
        this.taskStatus = webDownloadItem.getState();
      })
      // DocsCode 1735041667172
      // DocsCode 1735041674564
      // src/main/ets/utils/WebDownloadManager.ets

      this.delegate.onDownloadFailed((webDownloadItem: webview.WebDownloadItem) => {
        // Serialize the download data when the download fails to resume the download.
        this.failedData = webDownloadItem.serialize();
        this.taskStatus = webDownloadItem.getState();
        reject(new Error('download failed'));
      })

      // [Start delegate]
      // src/main/ets/utils/WebDownloadManager.ets

      this.delegate.onDownloadFinish((webDownloadItem: webview.WebDownloadItem) => {
        // After the download is complete, execute the custom logic.
        context.showToast({ message: $r('app.string.download_success') })
        // [StartExclude delegate]
        this.taskStatus = webDownloadItem.getState();
        resolve(this.sourcePath);
        // [EndExclude delegate]
      })
      // [End delegate]
      // Associate the download delegate with the webview controller.
      controller?.setDownloadDelegate(this.delegate);
      // src/main/ets/utils/WebDownloadManager.ets

      controller?.startDownload(url);
    });
  }

  public pause() {
    const state = this.currentTask.getState();
    if (state === webview.WebDownloadState.IN_PROGRESS) {
      this.currentTask.pause();
    }
  }

  public unpause() {
    const state = this.currentTask.getState();
    if (state === webview.WebDownloadState.PAUSED) {
      this.currentTask.resume();
    }
  }

  // DocsCode 1735041772212
  // src/main/ets/utils/WebDownloadManager.ets

  // Provides an external interface for resuming downloads.
  public resume() {
    const state = this.currentTask.getState();
    if (state === webview.WebDownloadState.CANCELED) {
      webview.WebDownloadManager.setDownloadDelegate(this.delegate);
      webview.WebDownloadManager.resumeDownload(webview.WebDownloadItem.deserialize(this.failedData));
    }
  }
  // DocsCode 1735041772212

  public cancel(): void {
    try {
      this.currentTask.cancel();
      this.taskStatus = this.currentTask.getState();
      this.deleteFile();
    } catch (e) {
      hilog.error(0x000, Constants.TAG, `failed to cancel：${JSON.stringify(e)}`);
    }
  }

  public clearProgress() {
    this.downloadSpeed = 0;
    this.downloadProgress = 0;
  }

  public deleteFile() {
    if (fileIo.accessSync(this.sourcePath)) {
      fileIo.unlink(this.sourcePath);
    }
  }

  public registerController(name: string, controller: webview.WebviewController): void {
    this.controllers.set(name, controller);
  }

  public getController(name: string): webview.WebviewController | undefined {
    return this.controllers.get(name);
  }
}