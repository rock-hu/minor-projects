#!/usr/bin/env python3
# coding: utf-8

"""
Copyright (c) 2023 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Description: entry to run sdk test daily
"""

import os
import subprocess

import utils


def run():
    cmd = ['python', 'run.py']
    cmd.extend(['--hapMode', 'all'])
    cmd.extend(['--compileMode', 'all'])
    cmd.extend(['--logLevel', 'debug'])
    cmd.extend(['--runHaps'])
    cmd.extend(['--logFile', 'log' + '_' + utils.get_time_string() + '.txt'])

    current_dir = os.path.dirname(os.path.abspath(__file__))
    print(current_dir)
    print(cmd)
    process = subprocess.Popen(cmd, cwd=current_dir,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)

    stdout, stderr = process.communicate(timeout=60 * 60 * 5)
    stdout_utf8 = stdout.decode("utf-8", errors="ignore")
    stderr_utf8 = stderr.decode("utf-8", errors="ignore")
    print(f"cmd stdout: {stdout_utf8}")
    print(f"cmd stderr: {stderr_utf8}")


if __name__ == '__main__':
    run()
