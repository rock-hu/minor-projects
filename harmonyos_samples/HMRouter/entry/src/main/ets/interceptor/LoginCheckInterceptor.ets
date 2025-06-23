/*
 *   Copyright (c) 2024 Huawei Device Co., Ltd.
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

/**
 * 最佳实践：基于HMRouter路由框架的页面跳转开发实践
 */

import { HMInterceptor, HMInterceptorAction, HMInterceptorInfo, HMRouterMgr, IHMInterceptor } from '@hadss/hmrouter';
import { LoginConstants } from '../component/common/constants/LoginConstants';

// [Start hm_interceptor]
@HMInterceptor({ interceptorName: 'LoginCheckInterceptor' })
export class LoginCheckInterceptor implements IHMInterceptor {
  handle(info: HMInterceptorInfo): HMInterceptorAction {
    // [StartExclude hm_interceptor]
    if (info.srcName === 'PayCard') {
      // [EndExclude hm_interceptor]
      if (!!AppStorage.get('isLogin')) {
        return HMInterceptorAction.DO_NEXT;
      } else {
        info.context.getPromptAction().showToast({ message: '请先登录' });
        HMRouterMgr.push({
          pageUrl: 'loginPage',
          skipAllInterceptor: true
        });
        return HMInterceptorAction.DO_REJECT;
      }
      // [StartExclude hm_interceptor]
    } else {
      if (!!AppStorage.get('isLogin')) {
        return HMInterceptorAction.DO_NEXT;
      } else {
        info.context.getPromptAction().showToast({ message: LoginConstants.LOGIN_TOAST });
        HMRouterMgr.push({
          pageUrl: 'loginPage',
          param: info.targetName,
          skipAllInterceptor: true
        });
        return HMInterceptorAction.DO_REJECT;
      }
    }
    // [EndExclude hm_interceptor]
  }
}

// [End hm_interceptor]