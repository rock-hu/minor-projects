#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
import argparse
import logging

parser = argparse.ArgumentParser(description="Sampler trace test")
parser.add_argument("--file", type=str)
args = parser.parse_args()
file_name = args.file

trace_list = [
    "LSamplerTest/ETSGLOBAL::execute_test; LSamplerTest/ETSGLOBAL::CallSlowFunction; "
    "LSamplerTest/ETSGLOBAL::SlowETSFunction;",
    "LSamplerTest/ETSGLOBAL::execute_test; LSamplerTest/ETSGLOBAL::CallSlowFunction; "
    "LSamplerTest/ETSGLOBAL::SlowETSFunctionCopy1;",
    "LSamplerTest/ETSGLOBAL::execute_test; LSamplerTest/ETSGLOBAL::CallSlowFunction; "
    "LSamplerTest/ETSGLOBAL::SlowETSFunctionCopy2;",
    "LSamplerTest/ETSGLOBAL::execute_test; LSamplerTest/ETSGLOBAL::CallSlowFunction; "
    "LSamplerTest/ETSGLOBAL::SlowETSFunctionCopy3;",
]

ALL_TRACES_FOUND = True

with open(file_name, 'r') as my_file:
    content = my_file.read()
    for string in trace_list:
        if string not in content:
            ALL_TRACES_FOUND = False
    if not ALL_TRACES_FOUND:
        logging.error("Actual stack trace")
        logging.error(content)
        raise Exception("Not all native traces found")
