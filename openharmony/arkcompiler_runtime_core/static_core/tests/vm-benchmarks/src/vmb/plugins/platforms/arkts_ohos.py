#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import logging
from vmb.plugins.platforms.arkts_device import Platform as ArkTS_hos
from vmb.target import Target

log = logging.getLogger('vmb')


class Platform(ArkTS_hos):

    @property
    def name(self) -> str:
        return 'ArkTS on OHOS Device'

    @property
    def target(self) -> Target:
        return Target.OHOS
