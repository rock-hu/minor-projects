#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

from runner.options.config import Config
from runner.plugins.declgents2ets.declgents2ets_suites import DeclgenTsSuites
from runner.plugins.declgents2ets.ts_test_dir import TsTestDir
from runner.plugins.ets.ets_test_suite import EtsTestSuite
from runner.plugins.ets.preparation_step import CtsTestPreparationStep
from runner.plugins.work_dir import WorkDir


class DeclgenCtsTsTestSuite(EtsTestSuite):
    def __init__(self, config: Config, work_dir: WorkDir, default_list_root: str):
        super().__init__(config, work_dir, DeclgenTsSuites.CTS.value, default_list_root)
        self._ts_test_dir = TsTestDir(
            config.general.static_core_root, config.general.test_root)
        self.set_preparation_steps()

    def set_preparation_steps(self) -> None:
        self._preparation_steps.append(CtsTestPreparationStep(
            test_source_path=self._ts_test_dir.ts_templates,
            test_gen_path=self.test_root,
            config=self.config
        ))
        