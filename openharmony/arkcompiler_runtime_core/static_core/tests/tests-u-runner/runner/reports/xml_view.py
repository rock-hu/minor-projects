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

from pathlib import Path
from typing import List, Set
import logging
import re

from runner.logger import Log
from runner.utils import write_2_file
from runner.reports.summary import Summary
from runner.test_base import Test
from runner.test_file_based import TestFileBased

_LOGGER = logging.getLogger("runner.reports.xml_view")


class XmlView:
    def __init__(self, report_root: Path, summary: Summary) -> None:
        self.__report_root = report_root
        self.__report_xml = 'report.xml'  # filename of xml report in junit format
        self.__ignore_list_xml = 'ignore.list'  # filename of list of ignored tests in the junit like format
        self.__summary = summary

    @staticmethod
    def remove_special_chars(data: str) -> str:
        xml_special_chars = {'&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": "&apos;"}
        for key, val in xml_special_chars.items():
            data = data.replace(key, val)
        return data

    def create_xml_report(self, results: List[Test], execution_time: float) -> None:
        total = self.__summary.passed + self.__summary.failed + self.__summary.ignored

        test_cases = []
        xml_failed = 0

        for test_result in results:
            if test_result.excluded:
                continue

            test_result_time = round(test_result.time if test_result.time else 0.0, 3)

            test_cases.append(
                f'<testcase classname="{self.__summary.name}" '
                f'name="{test_result.test_id}" '
                f'time="{test_result_time}">'
            )

            if not test_result.passed:
                xml_failed += 1
                failed_case = self.__get_failed_test_case(test_result)
                test_cases.extend(failed_case)

            test_cases.append('</testcase>')

        xml_report = [f'<testsuite name="{self.__summary.name}" '
                      f'tests="{total}" '
                      f'failures="{xml_failed}" '
                      f'time="{execution_time}">']
        xml_report.extend(test_cases)
        xml_report.append('</testsuite>\n')

        xml_report_path = self.__report_root / self.__report_xml
        Log.all(_LOGGER, f"Save {xml_report_path}")
        write_2_file(xml_report_path, "\n".join(xml_report))

    def create_ignore_list(self, tests: Set[Test]) -> None:
        result = [f"{self.__summary.name}.{test.test_id}" for test in tests if test.ignored]
        ignore_list_path = self.__report_root / self.__ignore_list_xml
        Log.all(_LOGGER, f"Save {ignore_list_path}")
        write_2_file(ignore_list_path, "\n".join(result))

    def __get_failed_test_case(self, test_result: Test) -> List[str]:
        if not isinstance(test_result, TestFileBased) or not test_result.report or test_result.fail_kind is None:
            return ['<failure>There are no data about the failure</failure>']

        result = []
        status = f"STATUS: {test_result.fail_kind.name}: "
        if test_result.report.output:
            status += str(re.sub(r'\[.*\]', '', test_result.report.output.splitlines()[0]))
        elif test_result.report.error:
            status += str(re.sub(r'\[.*\]', '', test_result.report.error.splitlines()[0]))

        result.append('<failure>')
        result.append('<![CDATA[')
        result.append(f"error kind = {test_result.fail_kind.name}")
        result.append(f"To reproduce:{test_result.reproduce}\n")
        result.append(f"OUT: {test_result.report.output}")
        result.append(f"ERR: {test_result.report.error}")
        result.append(f"return_code = {test_result.report.return_code}")
        result.append(status)
        result.append(']]>')
        result.append('</failure>')

        return result
