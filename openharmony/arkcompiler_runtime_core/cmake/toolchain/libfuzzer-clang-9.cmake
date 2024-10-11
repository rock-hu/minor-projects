# Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

# @HUAWEI(kwx790815): The project requirements must guarantee that we use appropriate compiler
# no need to specify a version then
set_c_compiler(clang-9)
set_cxx_compiler(clang++-9)

add_definitions(-DFUZZING_EXIT_ON_FAILED_ASSERT=1)
add_definitions(-DFUZZING_EXIT_ON_FAILED_ASSERT_FOR="/libpandafile/,/libpandabase/")

set(PANDA_BUILD_FUZZ_TARGETS true)
set(PANDA_ENABLE_LIBFUZZER true)
