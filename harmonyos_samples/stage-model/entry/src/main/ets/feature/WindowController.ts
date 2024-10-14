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

import { window,promptAction } from '@kit.ArkUI';
import Logger from '../util/Logger'

const TAG: string = 'WindowController'

let windowClass = null

export default class WindowController {

  // 获取当前应用内最后显示的窗口，使用callback异步回调。对应FA的getWindow
  getTopWindow(context) {
    window.getLastWindow(context, (err, data) => {
      if (err.code) {
        Logger.info(TAG, `Failed to obtain the top window. Cause: ${JSON.stringify(err)}`)
        promptAction.showToast({
          message: `Failed to obtain the top window. Cause: ${JSON.stringify(err)}`
        })
        return
      }
      Logger.info(TAG, `Succeeded in obtaining the top window. Data: ${JSON.stringify(data)}`)
      promptAction.showToast({
        message: `Succeeded in obtaining the top window`
      })
    })
  }

  // 设置当前能力的显示方向,对应FA模型的setDisplayOrientation()
  setPreferredOrientation(context) {

    window.getLastWindow(context, (err, data) => {
      if (err.code) {
        Logger.info(TAG, `Failed to obtain the top window. Cause: ${JSON.stringify(err)}`)
        promptAction.showToast({
          message: `Failed to obtain the top window. Cause: ${JSON.stringify(err)}`
        })
        return
      }
      windowClass = data
      let orientation = window.Orientation.AUTO_ROTATION
      let promise = windowClass.setPreferredOrientation(orientation)
      promise.then((data) => {
        Logger.info(TAG, `Succeeded in setting the window orientation. Data: ${JSON.stringify(data)}`)
        promptAction.showToast({
          message: `Succeeded in setting the window orientation`
        })
      }).catch((error) => {
        Logger.error(TAG, `Failed to set the window orientation. Cause: ${JSON.stringify(error)}`)
        promptAction.showToast({
          message: `Failed to set the window orientation. Cause: ${JSON.stringify(error)}`
        })
      })
    })
  }
}