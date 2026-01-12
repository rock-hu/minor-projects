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

import hilog from '@ohos.hilog'
import { Level } from '../Level';

export class HiLog {

    static log(level: string, message: string, tag: string) {
        switch (level) {
            case Level.TRACE.getLevelStr():
                hilog.info(0x0001, tag, message)
                break
            case Level.DEBUG.getLevelStr():
                hilog.debug(0x0001, tag, message)
                break
            case Level.INFO.getLevelStr():
                hilog.info(0x0001, tag, message)
                break
            case Level.WARN.getLevelStr():
                hilog.warn(0x0001, tag, message)
                break
            case Level.ERROR.getLevelStr():
                hilog.error(0x0001, tag, message)
                break
            case Level.FATAL.getLevelStr():
                hilog.fatal(0x0001, tag, message)
                break
        }
    }
}