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

from typing import Any

from runner.logger import Log

_LOGGER = Log.get_logger(__file__)


class CliArgsWrapper:
    args: dict[str, Any] | None = None  # type: ignore[explicit-any]

    @staticmethod
    def setup(args: dict[str, Any]) -> None:  # type: ignore[explicit-any]
        CliArgsWrapper.args = args
