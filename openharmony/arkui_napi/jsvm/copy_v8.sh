#!/bin/bash
# Copyright (c) 2024 Huawei Device Co., Ltd.
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

export SCRIPT_PATCH=$(dirname $(readlink -f "$0"))
export TARGET_GEN_DIR=$2
 
cp -u ${SCRIPT_PATCH}/../../../../third_party/node/deps/v8/libv8_shared.so ${TARGET_GEN_DIR}/libv8_shared.so
cp -r ${SCRIPT_PATCH}/../../../../third_party/node/deps/v8/include/v8-include ${TARGET_GEN_DIR}/v8-include
