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

class UIUtilsImpl {
    private static instance_: UIUtilsImpl = undefined;

    public getTarget<T extends Object>(source: T): T {
      if (!source || typeof source !== 'object') {
        return source;
      }
      if (ObservedObject.IsObservedObject(source)) {
        // V1 Proxy object
        return ObservedObject.GetRawObject(source);
      } else if (source[ObserveV2.SYMBOL_PROXY_GET_TARGET]) {
        // V2 Proxy object
        return source[ObserveV2.SYMBOL_PROXY_GET_TARGET];
      } else {
        // other situation, not handle
        return source;
      }
    }

    public makeObserved<T extends object>(target: T): T {
      // mark makeObserved using V2 feature
      ConfigureStateMgmt.instance.usingV2ObservedTrack('makeObserved', 'use')
      return RefInfo.get(target).proxy as T;
    }

    public static instance(): UIUtilsImpl {
      if (UIUtilsImpl.instance_) {
        return UIUtilsImpl.instance_;
      }
      UIUtilsImpl.instance_ = new UIUtilsImpl();
      return UIUtilsImpl.instance_;
    }
}