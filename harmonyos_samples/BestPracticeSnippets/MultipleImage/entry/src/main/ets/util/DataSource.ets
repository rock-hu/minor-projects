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

class DataSource implements IDataSource {
  private list: Array<PhotoData> = [];

  constructor(list: Array<PhotoData>) {
    this.list = list;
  }

  totalCount(): number {
    return this.list.length;
  }

  getData(index: number): PhotoData {
    return this.list[index];
  }

  registerDataChangeListener(listener: DataChangeListener): void {
  }

  unregisterDataChangeListener(listener: DataChangeListener): void {
  }
}

class PhotoData {
  total: number = 7;
  value: number = 0;
  id: number = -1;
}

export { DataSource, PhotoData }