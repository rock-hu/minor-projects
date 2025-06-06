/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { promptAction } from '@kit.ArkUI';
import { CommonConstant as Const } from '../constant/CommonConstant';
import NewsViewModel from '../../viewmodel/NewsViewModel';
import { NewsData } from '../../viewmodel/NewsData';
import NewsModel from '../../viewmodel/NewsModel';

const uiContext: UIContext | undefined = AppStorage.get('uiContext');

export function touchMoveLoadMore(that: NewsModel, event: TouchEvent) {
  if (that.endIndex === that.newsData.length - 1 || that.endIndex === that.newsData.length) {
    that.offsetY = event.touches[0].y - that.downY;
    if (Math.abs(that.offsetY) > uiContext!.vp2px(that.pullUpLoadHeight) / 2) {
      that.isCanLoadMore = true;
      that.isVisiblePullUpLoad = true;
      that.offsetY = -uiContext!.vp2px(that.pullUpLoadHeight) + that.offsetY * Const.Y_OFF_SET_COEFFICIENT;
    }
  }
}

export function touchUpLoadMore(that: NewsModel) {
  let self = that;
  uiContext!.animateTo({
    duration: Const.ANIMATION_DURATION,
  }, () => {
    self.offsetY = 0;
  })
  if ((self.isCanLoadMore === true) && (self.hasMore === true)) {
    self.isLoading = true;
    setTimeout(() => {
      closeLoadMore(that);
      NewsViewModel.getNewsList(self.currentPage, self.pageSize, Const.GET_NEWS_LIST).then((data: NewsData[]) => {
        if (data.length === self.pageSize) {
          self.currentPage++;
          self.hasMore = true;
        } else {
          self.hasMore = false;
        }
        self.newsData = self.newsData.concat(data);
      }).catch((err: string | Resource) => {
        uiContext!.getPromptAction().showToast({ message: err });
      })
    }, Const.DELAY_TIME);
  } else {
    closeLoadMore(self);
  }
}

export function closeLoadMore(that: NewsModel) {
  that.isCanLoadMore = false;
  that.isLoading = false;
  that.isVisiblePullUpLoad = false;
}