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

export interface CardItemInterface {
  icon: ResourceStr;
  name: ResourceStr;
  authorType: ResourceStr;
  content: ResourceStr;
}

export const CARD_LIST: CardItemInterface[] = [
  {
    icon: $r('app.media.ic_icon_1'),
    name: $r('app.string.name_1'),
    authorType: $r('app.string.author_type_1'),
    content: $r('app.string.content_1')
  },
  {
    icon: $r('app.media.ic_icon_2'),
    name: $r('app.string.name_2'),
    authorType: $r('app.string.author_type_1'),
    content: $r('app.string.content_2')
  },
  {
    icon: $r('app.media.ic_icon_2'),
    name: $r('app.string.name_2'),
    authorType: $r('app.string.author_type_1'),
    content: $r('app.string.content_3')
  },
  {
    icon: $r('app.media.ic_icon_3'),
    name: $r('app.string.name_3'),
    authorType: $r('app.string.author_type_2'),
    content: $r('app.string.content_4')
  },
  {
    icon: $r('app.media.ic_icon_4'),
    name: $r('app.string.name_4'),
    authorType: $r('app.string.author_type_3'),
    content: $r('app.string.content_5')
  },
  {
    icon: $r('app.media.ic_icon_2'),
    name: $r('app.string.name_5'),
    authorType: $r('app.string.author_type_4'),
    content: $r('app.string.content_6')
  },
  {
    icon: $r('app.media.ic_icon_1'),
    name: $r('app.string.name_1'),
    authorType: $r('app.string.author_type_1'),
    content: $r('app.string.content_1')
  },
  {
    icon: $r('app.media.ic_icon_2'),
    name: $r('app.string.name_2'),
    authorType: $r('app.string.author_type_1'),
    content: $r('app.string.content_2')
  },
  {
    icon: $r('app.media.ic_icon_3'),
    name: $r('app.string.name_3'),
    authorType: $r('app.string.author_type_2'),
    content: $r('app.string.content_4')
  },
  {
    icon: $r('app.media.ic_icon_4'),
    name: $r('app.string.name_4'),
    authorType: $r('app.string.author_type_3'),
    content: $r('app.string.content_5')
  },
  {
    icon: $r('app.media.ic_icon_2'),
    name: $r('app.string.name_5'),
    authorType: $r('app.string.author_type_4'),
    content: $r('app.string.content_6')
  }
]