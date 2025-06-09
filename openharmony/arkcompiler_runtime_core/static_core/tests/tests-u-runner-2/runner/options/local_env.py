#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

from typing import Optional, Dict, Any

from runner.logger import Log

_LOGGER = Log.get_logger(__file__)


class LocalEnv:
    __instance: Optional['LocalEnv'] = None

    def __init__(self) -> None:
        self.__env: Dict[str, Any] = {}

    @staticmethod
    def get() -> 'LocalEnv':
        if LocalEnv.__instance is None:
            LocalEnv.__instance = LocalEnv()
        return LocalEnv.__instance

    def add(self, key: str, env_value: str) -> None:
        if key not in self.__env:
            self.__env[key] = env_value
            return
        if key in self.__env and self.__env[key] != env_value:
            raise EnvironmentError(
                "Attention: environment has been changed during test execution. "
                f"For key `{key}` the env_value `{self.__env[key]}` changed to `{env_value}`"
            )

    def is_present(self, key: str) -> bool:
        return key in self.__env

    def get_value(self, key: str) -> str:
        return str(self.__env[key])

    def list(self) -> str:
        return str(self.__env)
