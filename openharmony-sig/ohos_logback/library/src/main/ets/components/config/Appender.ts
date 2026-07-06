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

import { Filter } from './Filter'
import { Encoder } from './Encoder'
import { RollingPolicy } from './RollingPolicy'
import { Discriminator } from './Discriminator'
import { Sift } from './Sift'

export type Appender = {
    name?: string
    className?: string
    file?: string
    encoder?: Encoder
    filter ?: Filter
    rollingPolicy ?: RollingPolicy
    discriminator?: Discriminator
    sift ?: Sift
    remoteHost?: string
    localHost?: string
    port?: number
    fromMail?: string
    toMail?: string
    subject?: string
    smtpHost?: string
    smtpPort?: number
    username?: string
    password?: string
    storeName?: string
}