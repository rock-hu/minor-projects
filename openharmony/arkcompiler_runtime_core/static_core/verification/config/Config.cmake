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

set(CONFIG_SOURCES
    ${VERIFICATION_SOURCES_DIR}/config/config_load.cpp
    ${VERIFICATION_SOURCES_DIR}/config/context/context.cpp
    ${VERIFICATION_SOURCES_DIR}/config/debug_breakpoint/breakpoint.cpp
    ${VERIFICATION_SOURCES_DIR}/config/debug_breakpoint/config_handler_breakpoints.cpp
    ${VERIFICATION_SOURCES_DIR}/config/default/default_config.cpp
    ${VERIFICATION_SOURCES_DIR}/config/handlers/config_handler_whitelist.cpp
    ${VERIFICATION_SOURCES_DIR}/config/handlers/config_handler_options.cpp
    ${VERIFICATION_SOURCES_DIR}/config/handlers/config_handler_method_options.cpp
    ${VERIFICATION_SOURCES_DIR}/config/handlers/config_handler_method_groups.cpp
    ${VERIFICATION_SOURCES_DIR}/config/options/method_options.cpp
    ${VERIFICATION_SOURCES_DIR}/config/parse/config_parse.cpp
    ${VERIFICATION_SOURCES_DIR}/config/process/config_process.cpp
    ${VERIFICATION_SOURCES_DIR}/config/whitelist/whitelist.cpp
)

set_source_files_properties(${VERIFICATION_SOURCES_DIR}/config/config/config_parse.cpp
    PROPERTIES COMPILE_FLAGS -fno-threadsafe-statics)

set_source_files_properties(${VERIFICATION_SOURCES_DIR}/config/handlers/config_handler_breakpoints.cpp
    PROPERTIES COMPILE_FLAGS -fno-threadsafe-statics)

set_source_files_properties(${VERIFICATION_SOURCES_DIR}/config/handlers/config_handler_whitelist.cpp
    PROPERTIES COMPILE_FLAGS -fno-threadsafe-statics)
