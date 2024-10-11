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

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

set(IS_USED_COMMON_COVERAGE_TOOLCHAIN true)

if(CMAKE_CXX_COMPILER_NAME)
    set_cxx_compiler(${CMAKE_CXX_COMPILER_NAME})
endif()

if(CMAKE_C_COMPILER_NAME)
    set_c_compiler(${CMAKE_C_COMPILER_NAME})
endif()

if(USE_GCOV_TOOLS AND USE_LLVM_COV_TOOLS)
    message(SEND_ERROR "Please do not use USE_GCOV_TOOLS and USE_LLVM_COV_TOOLS together" )
endif()

if(USE_GCOV_TOOLS)
    set(COVERAGE_FLAGS "-fprofile-arcs -ftest-coverage")
elseif(USE_LLVM_COV_TOOLS)
    set(COVERAGE_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_FLAGS}")
