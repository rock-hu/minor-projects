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

import argparse
import multiprocessing
from functools import cached_property
from pathlib import Path
from typing import Optional, Dict, Any, cast

from runner.common_exceptions import InvalidConfiguration
from runner.enum_types.qemu import QemuKind
from runner.enum_types.verbose_format import VerboseKind, VerboseFilter
from runner.options.macros import Macros
from runner.options.options import IOptions
from runner.options.options_coverage import CoverageOptions
from runner.options.options_time_report import TimeReportOptions
from runner.reports.report_format import ReportFormat
from runner.utils import convert_underscore


class GeneralOptions(IOptions):
    __DEFAULT_PROCESSES = 1
    __DEFAULT_CHUNKSIZE = 32
    __DEFAULT_GC_TYPE = "g1-gc"
    __DEFAULT_GC_BOMBING_FREQUENCY = 0
    __DEFAULT_HEAP_VERIFIER = "fail_on_verification"
    __DEFAULT_REPORT_FORMAT = ReportFormat.LOG
    __DEFAULT_DETAILED_REPORT_FILE = "detailed-report-file"
    __DEFAULT_VERBOSE = VerboseKind.SILENT
    __DEFAULT_VERBOSE_FILTER = VerboseFilter.NEW_FAILURES
    __DEFAULT_QEMU = QemuKind.NONE
    __DEFAULT_SHOW_PROGRESS = False
    __DEFAULT_DETAILED_REPORT = False
    __CFG_RUNNER = "runner"

    __VERBOSE = "verbose"
    __VERBOSE_FILTER = "verbose-filter"
    __PROCESSES = "processes"
    __CHUNKSIZE = "chunksize"
    __DETAILED_REPORT = "detailed-report"
    __DETAILED_REPORT_FILE = "detailed-report-file"
    __SHOW_PROGRESS = "show-progress"
    __QEMU = "qemu"

    def __init__(self, data: Dict[str, Any], parent: IOptions):
        super().__init__(data)
        self.__parameters: Dict[str, Any] = {}
        self._parent = parent
        for param_name, param_value in data.items():
            if param_name.startswith(self.__CFG_RUNNER):
                param_name = convert_underscore(param_name.replace(f"{self.__CFG_RUNNER}.", ""))
                self.__parameters[param_name] = param_value
        self.time_report = TimeReportOptions(self.__parameters)
        self.coverage = CoverageOptions(self.__parameters)

    def __str__(self) -> str:
        return self._to_str(indent=1)

    @staticmethod
    def add_cli_args(parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            f'--{GeneralOptions.__PROCESSES}', '-j', default=GeneralOptions.__DEFAULT_PROCESSES,
            help=f'Number of processes to use in parallel. By default {GeneralOptions.__DEFAULT_PROCESSES}. '
                 'Special value `all` - means to use all available processes')
        parser.add_argument(
            f'--{GeneralOptions.__DETAILED_REPORT}', action='store_true',
            default=GeneralOptions.__DEFAULT_DETAILED_REPORT,
            help='Create additional detailed report with counting tests for each folder.')
        parser.add_argument(
            f'--{GeneralOptions.__DETAILED_REPORT_FILE}', action='store',
            default=GeneralOptions.__DEFAULT_DETAILED_REPORT_FILE,
            help='Name of additional detailed report. By default, the report is created at '
                 '$WorkDir/<suite-name>/report/<suite-name>_detailed-report-file.md , '
                 'where $WorkDir is the folder specified by the environment variable WORK_DIR')
        parser.add_argument(
            f'--{GeneralOptions.__SHOW_PROGRESS}', action='store_true',
            default=GeneralOptions.__DEFAULT_SHOW_PROGRESS,
            help='Show progress bar')
        parser.add_argument(
            f'--{GeneralOptions.__VERBOSE}', '-v', action='store',
            default=GeneralOptions.__DEFAULT_VERBOSE,
            type=lambda arg: VerboseKind.is_value(arg, f"--{GeneralOptions.__VERBOSE}"),
            help='Enable verbose output. '
                 f'Possible values one of: {VerboseKind.values()}. Where '
                 'all - the most detailed output, '
                 'short - test status and output, '
                 'silent - only test status for new failed tests (by default)')
        parser.add_argument(
            f'--{GeneralOptions.__VERBOSE_FILTER}', action='store',
            default=GeneralOptions.__DEFAULT_VERBOSE_FILTER,
            type=lambda arg: VerboseFilter.is_value(arg, f"--{GeneralOptions.__VERBOSE_FILTER}"),
            help='Filter for what kind of tests to output stdout and stderr. Works only when --verbose option is set.'
                 f'Supported values: {VerboseFilter.values()}. Where '
                 'all - for all executed tests, '
                 'ignored - for new failures and tests from ignored test lists both passed and failed. '
                 'new - only for new failures (by default).')
        parser.add_argument(
            f'--{GeneralOptions.__QEMU}', action='store',
            default=GeneralOptions.__DEFAULT_QEMU,
            type=lambda arg: QemuKind.is_value(arg, f"--{GeneralOptions.__QEMU}"),
            help='Launch all binaries in qemu aarch64 (arm64) or arm (arm32)')

        TimeReportOptions.add_cli_args(parser)
        CoverageOptions.add_cli_args(parser)

    @cached_property
    def processes(self) -> int:
        procs = self.__parameters.get(self.__PROCESSES, self.__DEFAULT_PROCESSES)
        if isinstance(procs, str) and procs.lower() == "all":
            self.__parameters[self.__PROCESSES] = multiprocessing.cpu_count()
        return int(self.__parameters.get(self.__PROCESSES, self.__DEFAULT_PROCESSES))

    @cached_property
    def build(self) -> Path:
        build_path = Macros.expand_macros_in_path("${PANDA_BUILD}", self)
        if build_path is None:
            raise InvalidConfiguration(f"Build path is not set.")
        return Path(build_path).expanduser()

    @cached_property
    def chunksize(self) -> int:
        return GeneralOptions.__DEFAULT_CHUNKSIZE

    @cached_property
    def static_core_root(self) -> Path:
        panda_source_path = cast(str, Macros.expand_macros_in_path("${PANDA_SOURCE_PATH}", self))
        static_core_path = Path(panda_source_path).expanduser() / "runtime_core" / "static_core"
        return static_core_path

    @cached_property
    def show_progress(self) -> bool:
        return cast(bool, self.__parameters.get(self.__SHOW_PROGRESS, self.__DEFAULT_SHOW_PROGRESS))

    @cached_property
    def report_format(self) -> ReportFormat:
        return GeneralOptions.__DEFAULT_REPORT_FORMAT

    @cached_property
    def detailed_report(self) -> bool:
        return cast(bool, self.__parameters.get(self.__DETAILED_REPORT, self.__DEFAULT_DETAILED_REPORT))

    @cached_property
    def detailed_report_file(self) -> Optional[Path]:
        path_str = self.__parameters.get(self.__DETAILED_REPORT_FILE, self.__DEFAULT_DETAILED_REPORT_FILE)
        if path_str:
            return Path(cast(str, path_str))
        return None

    @cached_property
    def verbose(self) -> VerboseKind:
        kind = self.__parameters.get(self.__VERBOSE, self.__DEFAULT_VERBOSE)
        if isinstance(kind, VerboseKind):
            return kind
        raise ValueError("Verbose kind has incorrect value")

    @cached_property
    def verbose_filter(self) -> VerboseFilter:
        kind = self.__parameters.get(self.__VERBOSE_FILTER, self.__DEFAULT_VERBOSE_FILTER)
        if isinstance(kind, VerboseFilter):
            return kind
        raise ValueError("Verbose filter has incorrect value")

    @cached_property
    def qemu(self) -> QemuKind:
        kind = self.__parameters.get(self.__QEMU, self.__DEFAULT_QEMU)
        if isinstance(kind, QemuKind):
            return kind
        raise ValueError("Qemu kind has incorrect value")

    @cached_property
    def aot_check(self) -> bool:
        return False

    def get_command_line(self) -> str:
        options = [
            f'--processes={self.processes}' if self.processes != GeneralOptions.__DEFAULT_PROCESSES else '',
            f'--chunksize={self.chunksize}' if self.chunksize != GeneralOptions.__DEFAULT_CHUNKSIZE else '',
            '--show-progress' if self.show_progress else '',
            f'--verbose={self.verbose.value}'
            if self.verbose != GeneralOptions.__DEFAULT_VERBOSE else '',
            f'--verbose-filter={self.verbose_filter.value}'
            if self.verbose_filter != GeneralOptions.__DEFAULT_VERBOSE_FILTER else '',
            self.coverage.get_command_line(),
            '--aot' if self.aot_check else '',
            self.time_report.get_command_line(),
        ]
        return ' '.join(options)
