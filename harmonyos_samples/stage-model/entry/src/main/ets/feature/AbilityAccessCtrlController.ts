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
import { abilityAccessCtrl,bundleManager } from '@kit.AbilityKit'
import { promptAction } from '@kit.ArkUI';
import Logger from '../util/Logger'

const TAG: string = 'AbilityAccessCtrlController'


export default class AbilityAccessCtrlController {

  // 校验应用是否授予权限，使用Promise方式异步返回结果。对应FA模型的verifyPermission()
  verifyAccessToken() {
    let AtManager = abilityAccessCtrl.createAtManager()
    let bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION;
    let bundledata
    try {
      bundledata = bundleManager.getBundleInfoForSelfSync(bundleFlags);
    } catch (err) {
      Logger.info(TAG, `getBundleInfoForSelfSync error: ${err}`)
    }
    let tokenID = bundledata.appInfo.accessTokenId
    let promise = AtManager.verifyAccessTokenSync(tokenID, "ohos.permission.GRANT_SENSITIVE_PERMISSIONS");
    promptAction.showToast({
      message: `promise: data: ${JSON.stringify(promise)}`
    })
  }
}