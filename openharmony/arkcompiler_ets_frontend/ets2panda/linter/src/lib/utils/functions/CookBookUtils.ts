/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

export function extractRuleTags(tags: string[]): Map<number, string> {
  const resultMap = new Map<number, string>();

  for (let i = 0; i < tags.length; i++) {
    const tag = tags[i];

    if (!tag?.trim()) {
      continue;
    }

    const regex = /\(([^)]+)\)/;
    const match = tag.match(regex);

    if (match?.[1]?.trim()) {
      resultMap.set(i, match[1]);
    }
  }

  return resultMap;
}
