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

# At the moment, the script is nothing more than `diff` execution.

set -e

cross_values_1=$1
cross_values_2=$2

if [ $# -ne 2 ]; then
    printf "Invalid number of arguments\n"
    exit 1
fi

diff "${cross_values_1}" "${cross_values_2}"
res=$?

if [ ${res} -eq 1 ]; then
    printf "\nCheck failed: 'cross_values' are different:\n"
    printf "${cross_values_1}\n"
    printf "${cross_values_2}\n"
    exit ${res}
fi
exit ${res}
