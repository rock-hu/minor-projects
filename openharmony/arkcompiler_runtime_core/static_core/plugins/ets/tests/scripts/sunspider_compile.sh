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

if [[ -z "$1" || -z "$2" ]]; then
    echo "Build ark and es2panda. \n Provide args: ./sunspider_compile.sh build/path stdlib/path"
    exit 1
fi

build=$1
# Unused atm
#stdlib=$2

script_path=$(dirname "$0")
script_path=$(cd "$script_path" && pwd)
tests_path=$(cd "$script_path/../Sunspider" && pwd)

cd "$build"
"${build}/bin/es2panda" --extension=sts --output=etsstdlib.abc --gen-stdlib=true
for f in $tests_path/*.sts; do
    name=$(basename "$f" .sts)
    echo "$name"

    "${build}/bin/es2panda" --extension=sts --output=out.abc \
    --gen-stdlib=false "$f"

    "${build}/bin/ark" --boot-panda-files=etsstdlib.abc --load-runtimes=ets out.abc ETSGLOBAL::main

done
