#!/usr/bin/env bash
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

set -e

SCRIPT_DIR=$(realpath "$(dirname "${0}")")
ROOT_DIR=${STATIC_ROOT_DIR:-"${SCRIPT_DIR}/../.."}

RUNNER="${ROOT_DIR}/tests/tests-u-runner/runner_test.py"

source "${ROOT_DIR}/scripts/python/venv-utils.sh"
activate_venv
set +e

python3 -B "${RUNNER}"
EXIT_CODE=$?

set -e
deactivate_venv

exit ${EXIT_CODE}
