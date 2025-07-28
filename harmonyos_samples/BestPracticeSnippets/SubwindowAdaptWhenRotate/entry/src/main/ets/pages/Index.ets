/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

// [Start rotate_sample]
import { window } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';

const SUB_WINDOW_LEFT_OFFSET = 50;
const SUB_WINDOW_TOP_OFFSET = 500;

@Entry
@Component
struct Index {
  private mainWindow: window.Window | undefined = undefined;
  private subWindow: window.Window | undefined = undefined;

  aboutToAppear(): void {
    // create subWindow
    this.createSubWindow();

    this.mainWindow = AppStorage.get('mainWindow');
    if (!this.mainWindow) {
      return;
    }
    this.mainWindow.on('windowSizeChange', () => {
      this.adjustSubwindowSizeAndPosition();
    })
  }

  private adjustSubwindowSizeAndPosition() {
    if (!this.subWindow) {
      console.error('subWindow is null');
      return;
    }
    let subwindowRect: window.Rect = this.subWindow.getWindowProperties().windowRect;
    let newWidth: number = subwindowRect.height;
    let newHeight: number = subwindowRect.width;
    let newX: number = subwindowRect.top;
    let newY: number = subwindowRect.left;
    try {
      let promise = this.subWindow.resize(newWidth, newHeight);
      promise.then(() => {
        console.info('Succeeded in changing the window size');
      }).catch((err: BusinessError) => {
        console.error(`Failed to change the window size. Cause code: ${err.code}, message: ${err.message}`);
      });
    } catch (exception) {
      console.error(`Failed to change the window size. Cause code: ${exception.code}, message: ${exception.message}`);
    }

    try {
      let promise = this.subWindow.moveWindowTo(newX, newY);
      promise.then(() => {
        console.info('Succeeded in moving the window');
      }).catch((err: BusinessError) => {
        console.error(`Failed to move the window. Cause code: ${err.code}, message: ${err.message}`);
      });
    } catch (exception) {
      console.error(`Failed to move the window. Cause code: ${exception.code}, message: ${exception.message}`);
    }
  }

  // [StartExclude rotate_sample]
  private createSubWindow() {
    window.createWindow({
      name: 'subWindow',
      windowType: window.WindowType.TYPE_DIALOG,
      ctx: this.getUIContext().getHostContext(),
    }).then((subWindow) => {
      this.subWindow = subWindow;

      if (!this.mainWindow) {
        return;
      }
      let mainWindowRect: window.Rect = this.mainWindow.getWindowProperties().windowRect;
      let mainWindowWidth: number = mainWindowRect.width;
      let mainWindowHeight: number = mainWindowRect.height;
      this.subWindow.resize(mainWindowWidth - SUB_WINDOW_LEFT_OFFSET, mainWindowHeight - SUB_WINDOW_TOP_OFFSET);
      this.subWindow.moveWindowTo(SUB_WINDOW_LEFT_OFFSET / 2, SUB_WINDOW_TOP_OFFSET / 2);
      this.subWindow.setUIContent('pages/Subwindow').then(() => {
        this.subWindow?.setWindowBackgroundColor('#ff0000');
        this.subWindow?.showWindow().then(() => {
          console.info('Show subwindow success');
        }).catch((error: BusinessError) => {
          console.error(`Failed to show subwindow, error: ${error}`);
        });
      }).catch((error: BusinessError) => {
        console.error(`Failed to setUIContent, error: ${error}`);
      });
    }).catch((error: BusinessError) => {
      console.error(`Failed to create subwindow, error: ${error}`);
    });
  }

  build() {
    Column() {
    }
    .height('100%')
    .width('100%')
  }

  // [EndExclude rotate_sample]
}

// [End rotate_sample]