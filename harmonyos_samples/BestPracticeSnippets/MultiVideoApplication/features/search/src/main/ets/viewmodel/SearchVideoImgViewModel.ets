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

import { BreakpointConstants } from '@ohos/commons';
import { SearchConstants } from '../constants/SearchConstants';

export class SearchVideoImgModel {
  private trendingSearchList: SearchVideoImg[] = [];
  private searchResultImgList: SearchVideoImg[] = [];

  constructor() {
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_1'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[0], SearchConstants.SEARCH_IMG_IMG_CONTENTS[0], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_2'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[1], SearchConstants.SEARCH_IMG_IMG_CONTENTS[1], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_3'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[2], SearchConstants.SEARCH_IMG_IMG_CONTENTS[2], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_4'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[3], SearchConstants.SEARCH_IMG_IMG_CONTENTS[3], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_5'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[4], SearchConstants.SEARCH_IMG_IMG_CONTENTS[4], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_6'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[5], SearchConstants.SEARCH_IMG_IMG_CONTENTS[5], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_7'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[6], SearchConstants.SEARCH_IMG_IMG_CONTENTS[6], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_8'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[7], SearchConstants.SEARCH_IMG_IMG_CONTENTS[7], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_9'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[8], SearchConstants.SEARCH_IMG_IMG_CONTENTS[8], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_10'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[9], SearchConstants.SEARCH_IMG_IMG_CONTENTS[9], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_11'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[10], SearchConstants.SEARCH_IMG_IMG_CONTENTS[10], ''));
    this.trendingSearchList.push(new SearchVideoImg($r('app.media.search_video_12'),
      SearchConstants.SEARCH_IMG_IMG_TITLES[11], SearchConstants.SEARCH_IMG_IMG_CONTENTS[11], ''));
    this.searchResultImgList.push(new SearchVideoImg($r('app.media.search_result_1'),
      SearchConstants.SEARCH_RESULT_TITLES[0], SearchConstants.SEARCH_RESULT_CONTENTS[0],
      SearchConstants.SEARCH_RESULT_MORE[0]));
    this.searchResultImgList.push(new SearchVideoImg($r('app.media.search_result_2'),
      SearchConstants.SEARCH_RESULT_TITLES[1], SearchConstants.SEARCH_RESULT_CONTENTS[1],
      SearchConstants.SEARCH_RESULT_MORE[1]));
    this.searchResultImgList.push(new SearchVideoImg($r('app.media.search_result_3'),
      SearchConstants.SEARCH_RESULT_TITLES[2], SearchConstants.SEARCH_RESULT_CONTENTS[2],
      SearchConstants.SEARCH_RESULT_MORE[2]));
  }

  getTrendSearchList(currentWidthBreakpoint: string): SearchVideoImg[] {
    if (currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM) {
      return this.trendingSearchList.slice(0, 5);
    } else if (currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD) {
      return this.trendingSearchList.slice(0, 10);
    } else {
      return this.trendingSearchList;
    }
  }

  getSearchResultImgList(): SearchVideoImg[] {
    return this.searchResultImgList;
  }
}

export class SearchVideoImg {
  /**
   * Image src of the video.
   */
  private imgSrc: Resource;

  /**
   * Title of the video.
   */
  private title: ResourceStr;

  /**
   * Content of the video.
   */
  private content: ResourceStr;

  /**
   * Other information of the video.
   */
  private otherInfo: ResourceStr;

  constructor(imgSrc: Resource, title: ResourceStr, content: ResourceStr, otherInfo: ResourceStr) {
    this.imgSrc = imgSrc;
    this.title = title;
    this.content = content;
    this.otherInfo = otherInfo;
  }

  getImgSrc(): Resource {
    return this.imgSrc;
  }

  getTitle(): ResourceStr {
    return this.title;
  }

  getContent(): ResourceStr {
    return this.content;
  }

  getOtherInfo(): ResourceStr {
    return this.otherInfo;
  }
}