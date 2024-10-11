#!/usr/bin/env bash
# Copyright (c) 2024 Huawei Device Co., Ltd.
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

set -e

if [[ -z "$1" ]]; then
    echo "Usage: $0 <sources>"
    echo "    <sources> path where the sources to be checked are located"
    exit 1
fi

root_dir=$(realpath "$1")
# For pylint, we only enable certain rules
# - C0121 - comparison to True / False / None
# - C0123 - use instanceof() instead of type
# - C0304 - final newline
# - C0305 - trailing newlines
# - C0321 - one statement in a line
# - C0410 - one import in a line
# - C0411 - import ordering
# - C1801 - Do not use `len(SEQUENCE)` to determine if a sequence is empty
# - E0303 - __len__ method returns something which is not a non-negative integer
# - E0304 - __bool__ doesn't return bool
# - E0701 - bad 'except' order
# - E1111 - Assigning to function call which doesn’t return 
# - R1708 - do not raise StopIteration in the generator
# - R1710 - number and types of return values in all function branches should be the same
# - W0101 - no dead code
# - W0102 - do not use variable objects as default parameters
# - W0109 - keys in dict must be unique
# - W0123 - no eval() or exec() for untrusted code
# - W0150 - no return/break/continue in the final block
# - W0201 - do not define class properties outside __init__
# - W0212 - avoid access to protected members
# - W0221 - do not changes signature when overriding method
# - W0231 - correctly call __init__ of parent
# - W0601 - 'global' shoul reference existing variables
# - W0640 - do not use variables deined in outer loop
# - W0706 - except handler raises immediately
# - W1201 - string interpolation for logging
# - W3101 - timeout is used in certain functions

PYLINT_RULES=${PYLINT_RULES:-"C0121,C0123,C0304,C0305,C0321,C0410,\
C0411,C1801,E0303,E0304,E0701,E1111,R1708,R1710,W0101,W0102,W0109,\
W0123,W0150,W0201,W0212,W0221,W0231,W0601,W0706,W0640,W1201,W3101"}

# flake8 rules - can't be checked by pylint
# - S506 - unsafe-yaml-load
# - S602 - subprocess popen with shell
# - C101 - coding magic comment required
# - EXE022 - executable files have shebang 
FLAKE8_RULES=${FLAKE8_RULES:-"C101,EXE002,S506,S602"}

function save_exit_code() {
    return $(($1 + $2))
}

source "${root_dir}/scripts/python/venv-utils.sh"
activate_venv

set +e

EXIT_CODE=0

skip_options="^${root_dir}/third_party/"
if [ -n "$SKIP_FOLDERS" ]; then
    for pt in $SKIP_FOLDERS; do
        skip_options="${skip_options}\|^${root_dir}/${pt}/"
    done
fi

# Check all files with '.py' extensions except *conf.py which are config files for sphinx
while read file_to_check; do
    pylint -s false --timeout-methods subprocess.Popen.communicate --disable=all -e "${PYLINT_RULES}" "${file_to_check}"
    save_exit_code ${EXIT_CODE} $?
    EXIT_CODE=$?
    flake8 --select "${FLAKE8_RULES}" "${file_to_check}"
    save_exit_code ${EXIT_CODE} $?
    EXIT_CODE=$?
done <<<$(find "${root_dir}" -name "*.py" -type f | grep -v "${skip_options}")

num_checked=$(find "${root_dir}" -name "*.py" -type f | grep -c -v "${skip_options}")
echo "Checked ${num_checked} files"

deactivate_venv
exit ${EXIT_CODE}
