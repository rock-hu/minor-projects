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

set(PANDA_TRIPLET arm-linux-gnueabi)
set(PANDA_SYSROOT /usr/${PANDA_TRIPLET})
set(PANDA_TARGET_ARM32_ABI_SOFTFP 1)
set(PANDA_QEMU_BUILD 1)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_PREFIX_PATH ${PANDA_SYSROOT})

add_compile_options(
   -mfloat-abi=softfp
   -Wno-psabi
)

include(${CMAKE_CURRENT_LIST_DIR}/arm.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)
set_c_compiler(${PANDA_TRIPLET}-gcc)
set_cxx_compiler(${PANDA_TRIPLET}-g++)
set(PANDA_RUN_PREFIX ${QEMU_ARM32_BIN} -L ${PANDA_SYSROOT})
