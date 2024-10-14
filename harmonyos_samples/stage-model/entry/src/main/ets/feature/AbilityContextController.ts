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

import { abilityAccessCtrl,common } from '@kit.AbilityKit'
import { promptAction } from '@kit.ArkUI';
import Logger from '../util/Logger'

const TAG: string = 'AbilityContextController'
const accountId = 100
const resultCode = 100
const connectionNumber = 0
const permissions = ['']

let want = {
  bundleName: 'ohos.samples.stagemodel',
  abilityName: 'JumpAbility',
  moduleName: 'entry'
};

let serviceWant = {
  deviceId: '',
  bundleName: 'ohos.samples.stagemodel',
  abilityName: 'ServiceExtAbility',
};

export default class AbilityContextController {
  private context: common.UIAbilityContext

  constructor(context: common.UIAbilityContext) {
    this.context = context
  }

  private regOnRelease(caller) {
    try {
      caller.onRelease((msg) => {
        Logger.info(TAG, `caller onRelease is called ${msg}`)
      })
      Logger.info(TAG, 'caller register OnRelease succeed')
    } catch (error) {
      Logger.error(TAG, `caller register OnRelease failed with ${error}`)
    }
  }

  // 启动Ability，对应FA的StartServiceAbility
  startAbility() {
    this.context.startAbility(want, (error) => {
      Logger.info(TAG, `error.code: ${JSON.stringify(error.code)}`)
    })
  }

  // 启动Ability并在结束的时候返回执行结果，对应FA的startAbilityForResult
  startAbilityForResult() {
    this.context.startAbilityForResult(
      {
        deviceId: '', bundleName: 'ohos.samples.stagemodel', abilityName: 'JumpAbility'
      },
      (error, result) => {
        Logger.info(TAG, `startAbilityForResult AsyncCallback is called, error.code: ${JSON.stringify(error)}`)
        Logger.info(TAG, `startAbilityForResult AsyncCallback is called, result.resultCode: ${JSON.stringify(result.resultCode)}`)
      }
    );
  }

  // 停止Ability自身，对应FA的terminateSelf
  terminateSelf() {
    this.context.terminateSelf((error) => {
      Logger.info(TAG, `terminateSelf result: ${JSON.stringify(error)}`)
    })
  }

  // 停止Ability，配合startAbilityForResult使用，返回给接口调用方AbilityResult信息，对应FA的terminateSelfWithResult
  terminateSelfWithResult() {
    const abilityResult = {
      want,
      resultCode
    }
    this.context.terminateSelfWithResult(abilityResult, (error) => {
      Logger.info(TAG, `terminateSelfWithResult is called: ${JSON.stringify(error.code)}`)
    }
    )
  }

  // 断开连接，对应FA模型的disconnectService
  disconnectAbility() {
    this.context.disconnectServiceExtensionAbility(connectionNumber).then((data) => {
      Logger.info(TAG, `disconnectAbility success, data: ${JSON.stringify(data)}`)
      promptAction.showToast({
        message: 'disconnectAbility success'
      })
    }).catch((error) => {
      Logger.error(TAG, `disconnectAbility fail, error: ${JSON.stringify(error)}`)
      promptAction.showToast({
        message: 'disconnectAbility'
      })
    })
  }

  // 拉起弹窗请求用户授权，对应FA模型的AppContext中的requestPermissionsFromUser
  requestPermissionsFromUser() {
    let atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager()
    try {
      atManager.requestPermissionsFromUser(this.context, ['ohos.permission.ABILITY_BACKGROUND_COMMUNICATION']).then((data) => {
        Logger.info(TAG, `data: ${JSON.stringify(data)}`)
        promptAction.showToast({
          message: 'requestPermissionsFromUser success'
        })
      }).catch((err) => {
        Logger.info(TAG, `err: ${JSON.stringify(err)}`)
      })
    } catch (err) {
      Logger.info(TAG, `catch err->${JSON.stringify(err)}`);
    }
  }

  // 设置ability在任务中显示的名称，
  setMissionLabel() {
    this.context.setMissionLabel('test', (result) => {
      Logger.info(TAG, `setMissionLabel: ${JSON.stringify(result)}`)
      promptAction.showToast({
        message: 'setMissionLabel success'
      })
    })
  }

  // 查询ability是否在terminating状态。
  isTerminating() {
    const isTerminating = this.context.isTerminating()
    promptAction.showToast({
      message: 'isTerminating success'
    })
    Logger.info(TAG, `ability state: ${JSON.stringify(isTerminating)}`)
  }
}

