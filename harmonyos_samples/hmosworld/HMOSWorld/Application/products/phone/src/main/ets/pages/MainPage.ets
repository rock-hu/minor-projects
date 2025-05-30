/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

import { display } from '@kit.ArkUI';
import { common, ConfigurationConstant } from '@kit.AbilityKit';
import { fileIo } from '@kit.CoreFileKit';
import { image } from '@kit.ImageKit';
import { ConferenceView } from '@ohos/conference';
import { AudioPlayerService, AudioPlayerStatus } from '@ohos/audioplayer';
import { MapView } from '@ohos/challenge';
import { DiscoverView } from '@ohos/discover';
import { LearningView } from '@ohos/learning';
import {
  BreakpointSystem,
  BreakpointTypeEnum,
  ContinueModel,
  DarkModeUtil,
  FileUtil,
  GlobalContext,
  Logger,
  ResourcesType,
  WebUrls,
  WebUtil,
  WindowUtil
} from '@ohos/utils';
import { LearningResource, MineView, ProfileConstants, UserData, UserModel } from '@ohos/mine';
import { AppConstants } from '../constants/AppConstants';
import { CustomTabBar } from '../components/CustomTabBar';
import { TabBarType } from '../model/TabBarModel';

const TAG = '[MainPage]';
const context = (getContext(this) as common.UIAbilityContext);

@Entry
@Component
struct MainPage {
  @State currentIndex: TabBarType = TabBarType.DISCOVER;
  @State webViewController?: WebviewController = undefined;
  @State learnedId: string = '';
  @StorageLink('currentBreakpoint') currentBreakpoint: string = BreakpointTypeEnum.MD;
  private breakpointSystem: BreakpointSystem = new BreakpointSystem();
  @Provide('appPathStack') appPathStack: NavPathStack = new NavPathStack();
  @Provide('minePathStack') minePathStack: NavPathStack = new NavPathStack();
  @Provide('discoverPathStack') discoverPathStack: NavPathStack = new NavPathStack();
  @Provide('settingPathStack') settingPathStack: NavPathStack = new NavPathStack();

  aboutToAppear() {
    // Preloading the web page.
    WebUtil.createNWeb(WebUrls.ACTIVITY_URL, this.getUIContext());
    WebUtil.createNWeb(WebUrls.CHALLENGE_URL, this.getUIContext());
    this.breakpointSystem.register();
    try {
      if (canIUse('SystemCapability.Window.SessionManager')) {
        if (display.isFoldable()) {
          this.changeOrientation(display.getFoldStatus());
          display.on('foldStatusChange', (status: display.FoldStatus) => {
            this.changeOrientation(status);
          });
        }
      }
    } catch (exception) {
      Logger.error(TAG, 'Is not fold able device error: ' + JSON.stringify(exception));
    }
    // Set the current page to the transferable state.
    WindowUtil.setMissionContinueActive(context, true);

    if (ContinueModel.getInstance().isContinue) {
      const userData: UserData = ContinueModel.getInstance().data.userData as UserData;
      this.currentIndex = ContinueModel.getInstance().data.mainTabIndex as number;
      UserModel.getInstance().saveDistributedUserData(userData);
    }
    this.createProfile()
  }

  async createProfile() {
    // Copy the local avatar from the distributed file path to the app file path during transfer.
    const isDisProfileAccess = fileIo.accessSync(ProfileConstants.getInstance().DISTRIBUTED_PROFILE_PATH);
    Logger.info(TAG, 'isDisProfileAccess:' + isDisProfileAccess);
    if (isDisProfileAccess) {
      FileUtil.copyFile(ProfileConstants.getInstance().DISTRIBUTED_PROFILE_PATH,
        ProfileConstants.getInstance().LOCAL_PROFILE_PATH)
    }
    const isLocalProfileAccess = fileIo.accessSync(ProfileConstants.getInstance().LOCAL_PROFILE_PATH);
    Logger.info(TAG, 'isLocalProfileAccess:' + isLocalProfileAccess);
    if (isLocalProfileAccess) {
      const imageSource: image.ImageSource = image.createImageSource(ProfileConstants.getInstance().LOCAL_PROFILE_PATH);
      imageSource.createPixelMap((err, pixelMap) => {
        if (!err) {
          AppStorage.setOrCreate('profilePixelMap', pixelMap);
        }
        Logger.error(TAG, `LOCAL_PROFILE_PATH : ${JSON.stringify(err)}`);
      });
      await imageSource.release();
    }
  }

  changeOrientation(status: display.FoldStatus) {
    if (status !== display.FoldStatus.FOLD_STATUS_FOLDED) {
      AppStorage.setOrCreate('foldExpanded', true);
      WindowUtil.setMainWindowRestricted(getContext(this));
    } else {
      AppStorage.setOrCreate('foldExpanded', false);
      WindowUtil.setMainWindowPortrait(getContext(this));
    }
  }

  aboutToDisappear() {
    // Deregister the Listener.
    this.breakpointSystem.unregister();
    try {
      display.off('foldStatusChange');
      display.off('change');
    } catch (exception) {
      Logger.error(TAG, 'Failed to unregister callback. Code: ' + JSON.stringify(exception));
    }
    WindowUtil.setMissionContinueActive(context, false);
  }

  onPageShow() {
    DarkModeUtil.getInstance().checkDarkModeSetting();
    const jumpArticleId: string = GlobalContext.getContext().getObject('articleId') as string;
    // If the app is started from a service card or notification message, the corresponding article page is displayed.
    if (jumpArticleId) {
      GlobalContext.getContext().deleteObject('articleId');
      const article: LearningResource = new LearningResource();
      article.id = jumpArticleId;
      article.webUrl = GlobalContext.getContext().getObject('webUrl') as string;
      article.type = GlobalContext.getContext().getObject('type') as ResourcesType;
      this.appPathStack.pushPathByName('DiscoverArticleDetail',
        new Object({
          articleDetail: article,
          onDestinationBack: () => {
            this.appPathStack.pop();
            return false;
          }
        }));
    }
    if (this.currentIndex === TabBarType.MINE) {
      WindowUtil.updateStatusBarColor(getContext(this), true);
    } else {
      WindowUtil.updateStatusBarColor(getContext(this),
        AppStorage.get('currentColorMode') === ConfigurationConstant.ColorMode.COLOR_MODE_DARK);
    }
    Logger.info(TAG, 'onPageShow');
    if (AppStorage.get<boolean>('getHomeResource')) {
      this.currentIndex = TabBarType.DISCOVER;
    }
  }

  onBackPress(): boolean | void {
    if (this.currentIndex === TabBarType.ACTIVITY) {
      const webview = WebUtil.getController(WebUrls.ACTIVITY_URL);
      // If the web has level-2 page. the web page is returned and the return event of the current page is intercepted.
      if (webview?.accessBackward()) {
        webview.backward();
      }
      return true;
    }
  }

  build() {
    Navigation(this.appPathStack) {
      Stack() {
        if (this.currentBreakpoint === BreakpointTypeEnum.LG) {
          Image($r('app.media.ic_user_bg'))
            .width(AppConstants.FULL_PERCENT)
            .height(AppConstants.FULL_PERCENT)
        }

        Flex({
          direction: this.currentBreakpoint === BreakpointTypeEnum.LG ? FlexDirection.RowReverse : FlexDirection.Column,
        }) {
          Tabs({ index: this.currentIndex }) {
            TabContent() {
              DiscoverView()
            }

            TabContent() {
              LearningView({ learnedId: $learnedId })
            }

            TabContent() {
              if (canIUse('SystemCapability.Map.Core')) {
                MapView()
              }
            }

            TabContent() {
              ConferenceView()
            }

            TabContent() {
              MineView()
            }
          }
          .layoutWeight(1)
          .barHeight(0)
          .scrollable(false)
          .onChange((index) => {
            this.currentIndex = index;
            ContinueModel.getInstance().data.mainTabIndex = index;

            // Close Voice Player when switching tabs.
            if (AppStorage.get('audioPlayerStatus') !== AudioPlayerStatus.IDLE) {
              AudioPlayerService.getInstance().stop().then(() => {
                AudioPlayerService.destroy();
              });
            }
          })

          CustomTabBar({ currentIndex: $currentIndex })
        }
        .width(AppConstants.FULL_PERCENT)
        .height(AppConstants.FULL_PERCENT)
        .backgroundColor((this.currentBreakpoint === BreakpointTypeEnum.LG && this.currentIndex === TabBarType.MINE) ?
        $r('app.color.clear_color') : $r('sys.color.ohos_id_color_sub_background'))
      }
    }
    .hideTitleBar(true)
    .mode(NavigationMode.Stack)
  }
}