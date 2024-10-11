#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2023 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import time
import json
import stat
import argparse
import subprocess
import multiprocessing
import xml.etree.ElementTree as ET
from datetime import datetime


def parse_xml(xml_file_path):
    """
    Parse the XML file of the execution output of the use case
    """
    tree = ET.parse(xml_file_path)
    root = tree.getroot()
    tests = root.attrib.get("tests")
    failures = root.attrib.get("failures")
    failed_info = {
        "test_suite_name": [],
        "total_count": tests,
        "failed_count": failures,
        "failed_testcase_name": []
    }
    for testsuite in root.findall(".//testsuite"):
        testsuite_name = testsuite.attrib.get("name")
        testsuite_failures = testsuite.attrib.get("failures")
        if int(testsuite_failures):
            failed_info["test_suite_name"].append(testsuite_name)
        for testcase in testsuite.findall(".//testcase"):
            testcase_name = testcase.attrib.get("name")
            failure = testcase.find("failure")
            if failure is not None:
                failed_info["failed_testcase_name"].append(testcase_name)
    return failed_info


def run_command(test_binary_path: str, alter_cmds: list = None):
    """
    Run a gtest test binary.
    """
    default_cmds = []
    default_cmds.append(test_binary_path)
    default_cmds.append("--gtest_output=xml:{}.xml".format(test_binary_path))
    default_cmds.append("--gtest_print_time=0")
    default_cmds.append("--gtest_brief=1")
    if alter_cmds is not None:
        default_cmds.extend(alter_cmds)
    subprocess.run(default_cmds)


def run_single_test(tests_path, test_suite_name):
    """
    Run a gtest test suite
    """
    test_suite_path = None
    for root, _, files in os.walk(tests_path):
        for file in files:
            if file.endswith(test_suite_name):
                test_suite_path =  os.path.join(root, test_suite_name)
    if test_suite_path is not None:
        run_command(test_suite_path)
    else:
        print("TestSuite {} did not compile successfully.".format(test_suite_name))


def run_tests_parallel(test_directory):
    """
    Run all gtest test binaries in parallel.
    """
    test_binaries = []
    for root, _, files in os.walk(test_directory):
        for file in files:
            test_suite_path = os.path.join(root, file)
            name, ext = os.path.splitext(file)
            if ext == "":
                test_binaries.append(test_suite_path)
    start = time.time()
    with multiprocessing.Pool(processes=64) as pool:
        pool.map(run_command, iter(test_binaries))
    end = time.time()
    test_result = {
        "time_stamp": str(datetime.now()),
        "execute_time": 0,
        "total_execute_tests": 0,
        "failed_tests_count": 0,
        "unavailable": [],
        "failed": []
    }
    total_tests_count = 0
    failed_tests_count = 0
    for test_binary in test_binaries:
        xml_file_path = "{}.xml".format(test_binary)
        if os.path.exists(xml_file_path):
            failed_info = parse_xml(xml_file_path)
            total_tests_count = total_tests_count + int(failed_info.get('total_count', '0'))
            failed_tests_count = failed_tests_count + int(failed_info.get('failed_count', '0'))
            if int(failed_info.get('failed_count', '0')):
                test_result['failed'].append(failed_info)
        else:
            test_result["unavailable"].append(test_binary.split('/')[-1])
    test_result["execute_time"] = "{} seconds".format(round(end - start, 2))
    test_result['total_execute_tests'] = total_tests_count
    test_result['failed_tests_count'] = failed_tests_count
    json_file_path = os.path.join(test_directory, "test_result.json")
    flags = os.O_CREAT | os.O_WRONLY | os.O_TRUNC
    mode = stat.S_IRUSR | stat.S_IWUSR
    with os.fdopen(os.open(json_file_path, flags, mode), 'w') as json_file:
        json.dump(test_result, json_file, indent=2)

    print("The test results have been generated, path is {}".format(json_file_path))


def get_tests_out_path():
    """
    Obtain the output directory of test cases
    """
    code_path = os.getcwd()
    for _ in range(6):
        code_path = os.path.dirname(code_path)
    code_path = os.path.join(code_path, "out/rk3568/clang_x64/tests/unittest/ace_engine")
    return code_path


def main():
    """
    Add unitest case execution parameters
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("-t", "--target", nargs='+', type=str, default=None)
    tests_out_path = get_tests_out_path()
    args = parser.parse_args()
    targets = args.target
    if targets is not None:
        for target in targets:
            run_single_test(tests_out_path, target)
    else:
        run_tests_parallel(tests_out_path)


if __name__ == "__main__":
    main()
