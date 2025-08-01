/*
* Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { AbilityConstant, UIAbility, Want, EnvironmentCallback, ConfigurationConstant } from '@kit.AbilityKit';
import { commonType, distributedDataObject } from '@kit.ArkData';
import { fileIo as fs, fileUri } from '@kit.CoreFileKit';
import { display, KeyboardAvoidMode, window } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { image } from '@kit.ImageKit';
import { JSON } from '@kit.ArkTS';
import { BreakpointConstants } from '../constants/BreakpointConstants';
import { ContentInfo, ImageInfo } from '../model/ContentInfo';

export default class EntryAbility extends UIAbility {
  windowClass: window.Window | undefined = undefined;
  private windowObj?: window.Window;
  private curBp: string = '';
  private imageUriArray: Array<ImageInfo> = [];
  private distributedObject: distributedDataObject.DataObject | undefined = undefined;
  private callbackId: number = 0;

  // [Start distribute]
  // [Start on_create]
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, '[EntryAbility]', 'Ability onCreate');
    this.restoreDistributedObject(want, launchParam);
    AppStorage.setOrCreate('systemColorMode', this.context.config.colorMode);
    this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
    // [StartExclude distribute]
    let environmentCallback: EnvironmentCallback = {
      onConfigurationUpdated(config) {
        AppStorage.setOrCreate('language', config.language);
      },
      onMemoryLevel(level) {
        hilog.info(0x0000, '[EntryAbility]', `onMemoryLevel level: ${level}`);
      }
    };
    let applicationContext = this.context.getApplicationContext();
    try {
      this.callbackId = applicationContext.on('environment', environmentCallback);
    } catch (paramError) {
      hilog.error(0x0000, '[EntryAbility]',
        `error: ${(paramError as BusinessError).code}, ${(paramError as BusinessError).message}`);
    }
    hilog.info(0x0000, '[EntryAbility]', `registerEnvironmentCallback callbackId: ${this.callbackId}`);
    // [EndExclude distribute]
  }

  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, '[EntryAbility]', 'Ability onNewWant');
    this.restoreDistributedObject(want, launchParam);
  }

  /*
   * The peer device receives data.
   * @param want
   * @param launchParam
   * @returns
   */
  async restoreDistributedObject(want: Want, launchParam: AbilityConstant.LaunchParam): Promise<void> {
    if (launchParam.launchReason !== AbilityConstant.LaunchReason.CONTINUATION) {
      return;
    }

    let mailInfo: ContentInfo = new ContentInfo(undefined, undefined, [], undefined, undefined, undefined, undefined);
    this.distributedObject = distributedDataObject.create(this.context, mailInfo);
    // Add a data restored listener.
    this.distributedObject.on('status',
      (sessionId: string, networkId: string, status: 'online' | 'offline' | 'restored') => {
        hilog.info(0x0000, '[EntryAbility]', `status changed, sessionId: ${sessionId}`);
        hilog.info(0x0000, '[EntryAbility]', `status changed, status: ${status}`);
        hilog.info(0x0000, '[EntryAbility]', `status changed, networkId: ${networkId}`);
        if (status === 'restored') {
          if (!this.distributedObject) {
            return;
          }
          AppStorage.setOrCreate('mainTitle', this.distributedObject['mainTitle']);
          AppStorage.setOrCreate('textContent', this.distributedObject['textContent']);
          AppStorage.setOrCreate('isShowLocalInfo', this.distributedObject['isShowLocalInfo']);
          AppStorage.setOrCreate('isAddLocalInfo', this.distributedObject['isAddLocalInfo']);
          AppStorage.setOrCreate('selectLocalInfo', this.distributedObject['selectLocalInfo']);
          AppStorage.setOrCreate('attachments', this.distributedObject['attachments']);
          let attachments = this.distributedObject['attachments'] as commonType.Assets;
          hilog.info(0x0000, '[EntryAbility]', `attachments: ${JSON.stringify(this.distributedObject['attachments'])}`);
          for (const attachment of attachments) {
            this.fileCopy(attachment);
          }
          AppStorage.setOrCreate<Array<ImageInfo>>('imageUriArray', this.imageUriArray);
        }
      });
    let sessionId: string = want.parameters?.distributedSessionId as string;
    this.distributedObject.setSessionId(sessionId);
    this.context.restoreWindowStage(new LocalStorage());
  }

  // [End distribute]

  // [Start onContinue]
  // [End on_create]
  // [Start on_continue]
  async onContinue(wantParam: Record<string, Object | undefined>): Promise<AbilityConstant.OnContinueResult> {
    wantParam.imageUriArray = JSON.stringify(AppStorage.get<Array<PixelMap>>('imageUriArray'));
    try {
      // Generate the session ID of the distributed data object.
      let sessionId: string = distributedDataObject.genSessionId();
      wantParam.distributedSessionId = sessionId;

      let imageUriArray = AppStorage.get<Array<ImageInfo>>('imageUriArray');
      let assets: commonType.Assets = [];
      if (imageUriArray) {
        for (let i = 0; i < imageUriArray.length; i++) {
          let append = imageUriArray[i];
          let attachment: commonType.Asset = this.getAssetInfo(append);
          assets.push(attachment);
        }
      }

      let contentInfo: ContentInfo = new ContentInfo(
        AppStorage.get('mainTitle'),
        AppStorage.get('textContent'),
        AppStorage.get('imageUriArray'),
        AppStorage.get('isShowLocalInfo'),
        AppStorage.get('isAddLocalInfo'),
        AppStorage.get('selectLocalInfo'),
        assets
      );
      let source = contentInfo.flatAssets();
      this.distributedObject = distributedDataObject.create(this.context, source);
      this.distributedObject.setSessionId(sessionId);
      await this.distributedObject.save(wantParam.targetDevice as string).catch((err: BusinessError) => {
        hilog.info(0x0000, '[EntryAbility]', `Failed to save. Code: ${err.code}, message: ${err.message}`);
      });
    } catch (error) {
      hilog.error(0x0000, '[EntryAbility]', 'distributedDataObject failed', `code ${(error as BusinessError).code}`);
    }
    return AbilityConstant.OnContinueResult.AGREE;
  }

  // [End onContinue]

  // [Start fileCopy]
  // [End on_continue]
  // [Start file_copy]
  /*
   * Copy distributed files.
   * @param attachmentRecord
   * @param key
   */
  private fileCopy(attachment: commonType.Asset): void {
    if (canIUse('SystemCapability.DistributedDataManager.CommonType')) {
      let filePath: string = this.context.distributedFilesDir + '/' + attachment.name;
      let savePath: string = this.context.filesDir + '/' + attachment.name;
      try {
        if (fs.accessSync(filePath)) {
          let saveFile = fs.openSync(savePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
          let file = fs.openSync(filePath, fs.OpenMode.READ_WRITE);
          let buf: ArrayBuffer = new ArrayBuffer(Number(attachment.size));
          let readSize = 0;
          let readLen = fs.readSync(file.fd, buf, {
            offset: readSize
          });
          let sourceOptions: image.SourceOptions = {
            sourceDensity: 120
          };
          let imageSourceApi: image.ImageSource = image.createImageSource(buf, sourceOptions);
          this.imageUriArray.push({
            imagePixelMap: imageSourceApi.createPixelMapSync(),
            imageName: attachment.name
          });
          while (readLen > 0) {
            readSize += readLen;
            fs.writeSync(saveFile.fd, buf);
            readLen = fs.readSync(file.fd, buf, {
              offset: readSize
            });
          }
          fs.closeSync(file);
          fs.closeSync(saveFile);
          hilog.info(0x0000, '[EntryAbility]', attachment.name + 'synchronized successfully.');
        }
      } catch (error) {
        let err: BusinessError = error as BusinessError;
        hilog.error(0x0000, '[EntryAbility]', `DocumentViewPicker failed with err: ${JSON.stringify(err)}`);
      }
    }
  }

  // [End fileCopy]
  // [End file_copy]
  /*
   * Obtain distributed file asset information.
   * @param append
   * @returns
   */
  private getAssetInfo(append: ImageInfo): commonType.Asset {
    let filePath = this.context.distributedFilesDir + '/' + append.imageName;
    fs.statSync(filePath);
    let uri: string = fileUri.getUriFromPath(filePath);
    let stat = fs.statSync(filePath);
    let attachment: commonType.Asset = {
      name: append.imageName,
      uri: uri,
      path: filePath,
      createTime: stat.ctime.toString(),
      modifyTime: stat.ctime.toString(),
      size: stat.size.toString()
    };
    hilog.info(0x0000, '[EntryAbility]', `[getAssetInfo] attachments: ${JSON.stringify(attachment)}`);
    return attachment;
  }

  /*
   * Update breakpoints based on the current window size.
   */
  private updateBreakpoint(windowWidth: number): void {
    // Get the current window object and obtain the current displayId
    let displayId = this.windowObj?.getWindowProperties().displayId;
    try {
      // Convert the unit of length from px to vp.
      let windowWidthVp = windowWidth / display.getDisplayByIdSync(displayId).densityPixels;
      let newBp: string = '';
      if (windowWidthVp < 320) {
        newBp = BreakpointConstants.BREAKPOINT_XS;
      } else if (windowWidthVp < 600) {
        newBp = BreakpointConstants.BREAKPOINT_SM;
      } else if (windowWidthVp < 840) {
        newBp = BreakpointConstants.BREAKPOINT_MD;
      } else {
        newBp = BreakpointConstants.BREAKPOINT_LG;
      }
      if (this.curBp !== newBp) {
        this.curBp = newBp;
        // Record the current breakpoint value using the state variable.
        AppStorage.setOrCreate('currentBreakpoint', this.curBp);
      }
    } catch (err) {
      hilog.info(0x0000, '[EntryAbility]', 'getDisplayByIdSync failed err' + err.code);
    }
  }

  onWindowStageRestore(windowStage: window.WindowStage): void {
    hilog.info(0x0000, '[EntryAbility]', 'Ability onWindowStageRestore');
    windowStage.loadContent('pages/Home', (err, data) => {
      if (err.code) {
        hilog.error(0x0000, '[EntryAbility]', 'Failed to load the content, ', `Catch err: ${err}`);
        return;
      }
      windowStage.getMainWindowSync().setWindowLayoutFullScreen(true);
      windowStage.getMainWindowSync().getUIContext().getKeyboardAvoidMode();
      windowStage.getMainWindowSync().getUIContext().setKeyboardAvoidMode(KeyboardAvoidMode.RESIZE);
      this.setImmersion(windowStage);
      hilog.info(0x0000, '[EntryAbility]', 'Succeeded in loading the content, ', `Data: ${data}`);
    });
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability.
    hilog.info(0x0000, '[EntryAbility]', 'Ability onWindowStageCreate');
    windowStage.getMainWindow().then((windowObj) => {
      this.windowObj = windowObj;
      // Gets the size of the window when the application starts.
      this.updateBreakpoint(windowObj.getWindowProperties().windowRect.width);
      // Register callback functions that listen for window size changes.
      windowObj.on('windowSizeChange', (windowSize) => {
        this.updateBreakpoint(windowSize.width);
      });
    });
    windowStage.loadContent('pages/Home', (err, data) => {
      if (err.code) {
        hilog.error(0x0000, '[EntryAbility]', 'Failed to load the content, ', `Catch err: ${err}`);
        return;
      }
      windowStage.getMainWindowSync().setWindowLayoutFullScreen(true);
      windowStage.getMainWindowSync().getUIContext().getKeyboardAvoidMode();
      windowStage.getMainWindowSync().getUIContext().setKeyboardAvoidMode(KeyboardAvoidMode.RESIZE);
      this.setImmersion(windowStage);
      hilog.info(0x0000, '[EntryAbility]', 'Succeeded in loading the content, ', `Data: ${data}`);
    });
  }

  setImmersion(windowStage: window.WindowStage): void {
    windowStage.getMainWindow((err: BusinessError, data) => {
      const errCode: number = err.code;
      if (errCode) {
        hilog.error(0x0000, '[EntryAbility]', 'Failed to obtain the main window. Cause: ' + JSON.stringify(err));
        return;
      }
      this.windowClass = data;
      try {
        let avoidArea = this.windowClass.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM);
        // Set the height of the status bar.
        AppStorage.setOrCreate('statusBarHeight', data.getUIContext().px2vp(avoidArea.topRect.height));
        AppStorage.setOrCreate('bottomHeight', data.getUIContext()
          .px2vp(this.windowClass.getWindowAvoidArea(window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR)
            .bottomRect
            .height));
      } catch (exception) {
        hilog.error(0x0000, '[EntryAbility]',
          `Failed to set the system bar to be invisible. Cause code: ${exception.code},
        message: ${exception.message}`);
      }
    })
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(0x0000, '[EntryAbility]', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground
    hilog.info(0x0000, '[EntryAbility]', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background
    hilog.info(0x0000, '[EntryAbility]', 'Ability onBackground');
  }

  onDestroy(): void {
    hilog.info(0x0000, '[EntryAbility]', 'Ability onDestroy');
  }
}