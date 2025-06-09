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

from dataclasses import dataclass
from pathlib import Path
from typing import Any, List, Set, Optional, Dict

from runner.code_coverage.coverage import LlvmCov
from runner.options.config import Config
from runner.reports.report_format import ReportFormat
from runner.suites.work_dir import WorkDir


@dataclass
class TestEnv:
    config: Config

    cmd_prefix: List[str]
    cmd_env: Dict[str, str]

    timestamp: int
    report_formats: Set[ReportFormat]
    work_dir: WorkDir

    coverage: Optional[LlvmCov] = None

    util: Any = None


@dataclass(frozen=True)
class BinaryParams:
    timeout: int
    executor: Path
    flags: List[str]
    env: Dict[str, str]


@dataclass
class TestReport:
    output: str
    error: str
    return_code: int
