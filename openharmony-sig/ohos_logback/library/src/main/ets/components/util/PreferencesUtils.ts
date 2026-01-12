/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import data_preferences from '@ohos.data.preferences';
import { Context } from '@ohos.abilityAccessCtrl';

export class PreferencesUtils {
    static CURRENT_TIME: string = 'currentTime'

    static putData(context: Context, preferencesName: string, key: string, data: number): Promise<void> {
        return new Promise<void>(function (resolve, reject) {
          if(context)
            data_preferences.getPreferences(context, preferencesName)
                .then((preferences) => {
                    preferences.put(key, data)
                        .then(() => {
                            resolve()
                        }).catch(() => {
                        reject()
                    })
                }).catch(() => {
                reject()
            })
          else reject()
        });
    }

    static getData(context: Context, preferencesName: string, key: string, defaultData: number): Promise<number> {
        return new Promise<number>(function (resolve, reject) {
          if(context)
            data_preferences.getPreferences(context, preferencesName)
                .then((preferences) => {
                    preferences.get(key, defaultData)
                        .then((value) => {
                            console.info('TianHong getData value 1111111 = ' + value)
                            resolve(value as number)
                        }).catch(() => {
                        reject()
                    })
                }).catch(() => {
                reject()
            })
          else reject()
        });
    }
}
