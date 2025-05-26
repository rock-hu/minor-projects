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

/**
 * Constants.
 */
export interface ImageData {
  src: Resource,
  selected: boolean
}

export class AlbumViewConstants {
  /**
   * Image list.
   */
  static readonly IMAGE_LIST: ImageData[] = [
    { src: $r("app.media.album_photo1"), selected: false },
    { src: $r("app.media.album_photo2"), selected: false },
    { src: $r("app.media.album_photo3"), selected: false },
    { src: $r("app.media.album_photo4"), selected: false },
    { src: $r("app.media.album_photo5"), selected: false },
    { src: $r("app.media.album_photo6"), selected: false },
    { src: $r("app.media.album_photo7"), selected: false },
    { src: $r("app.media.album_photo1"), selected: false },
    { src: $r("app.media.album_photo2"), selected: false },
    { src: $r("app.media.album_photo3"), selected: false },
    { src: $r("app.media.album_photo4"), selected: false },
    { src: $r("app.media.album_photo5"), selected: false },
    { src: $r("app.media.album_photo6"), selected: false },
    { src: $r("app.media.album_photo7"), selected: false },
    { src: $r("app.media.album_photo1"), selected: false },
    { src: $r("app.media.album_photo2"), selected: false },
    { src: $r("app.media.album_photo3"), selected: false },
    { src: $r("app.media.album_photo4"), selected: false },
    { src: $r("app.media.album_photo5"), selected: false },
    { src: $r("app.media.album_photo6"), selected: false },
    { src: $r("app.media.album_photo7"), selected: false },
    { src: $r("app.media.album_photo1"), selected: false },
    { src: $r("app.media.album_photo2"), selected: false },
    { src: $r("app.media.album_photo3"), selected: false },
    { src: $r("app.media.album_photo4"), selected: false },
    { src: $r("app.media.album_photo5"), selected: false },
    { src: $r("app.media.album_photo6"), selected: false },
    { src: $r("app.media.album_photo7"), selected: false },
    { src: $r("app.media.album_photo1"), selected: false },
    { src: $r("app.media.album_photo2"), selected: false },
    { src: $r("app.media.album_photo3"), selected: false },
    { src: $r("app.media.album_photo4"), selected: false },
    { src: $r("app.media.album_photo5"), selected: false },
    { src: $r("app.media.album_photo6"), selected: false },
    { src: $r("app.media.album_photo7"), selected: false },
    { src: $r("app.media.album_photo1"), selected: false },
    { src: $r("app.media.album_photo2"), selected: false },
    { src: $r("app.media.album_photo3"), selected: false },
    { src: $r("app.media.album_photo4"), selected: false },
    { src: $r("app.media.album_photo5"), selected: false },
    { src: $r("app.media.album_photo6"), selected: false },
    { src: $r("app.media.album_photo7"), selected: false }
  ];
  /**
   * Default text.
   */
  static DES_TEXT = '图片';
  /**
   * Side constants.
   */
  static SideConstants: string[] = ['    照片、人物、地点...', '相册', '图片', '我的收藏', '12', '视频', '新建相册1', '新建相册1', '人像', '地点', '上海市', '北京市', '南京市', '分类'];
}