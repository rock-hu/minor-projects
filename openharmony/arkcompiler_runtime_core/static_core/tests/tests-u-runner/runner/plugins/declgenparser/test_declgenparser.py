#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
from typing import List, Any

from runner.test_file_based import TestFileBased
from runner.enum_types.params import TestEnv

_LOGGER = logging.getLogger("runner.plugins.declgenparser.test_declgenparser")


class TestDeclgenParser(TestFileBased):
    def __init__(self, test_env: TestEnv, test_path: str, flags: List[str], test_id: str) -> None:
        TestFileBased.__init__(self, test_env, test_path, flags, test_id)

    @staticmethod
    def es2panda_flags() -> List[str]:
        # we want "--opt-level=2", but currently it does not work with `--output=/dev/null`
        return []

    @staticmethod
    def es2panda_result_validator(_actual_output: str, _: Any, actual_return_code: int) -> bool:
        return actual_return_code == 0

    def do_run(self) -> TestFileBased:
        es2panda_flags = self.es2panda_flags()
        es2panda_flags.extend(self.flags)
        es2panda_flags.append("--output=/dev/null")
        self.passed, self.report, self.fail_kind = self.run_es2panda(
            flags=es2panda_flags,
            test_abc="",
            result_validator=self.es2panda_result_validator,
        )
        return self
