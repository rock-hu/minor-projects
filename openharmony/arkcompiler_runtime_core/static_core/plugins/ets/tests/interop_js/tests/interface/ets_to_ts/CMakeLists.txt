# Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

panda_ets_interop_js_gtest(ets_interop_js__interface_ets_to_ts
    CPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/interface_method.cpp
    ETS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/interface_method.ets
    JS_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/check_any_type_interface_method.js
        ${CMAKE_CURRENT_SOURCE_DIR}/check_literal_type_interface_method.js
        ${CMAKE_CURRENT_SOURCE_DIR}/check_optional_method_interface.js
        ${CMAKE_CURRENT_SOURCE_DIR}/check_subset_by_ref_interface_method.js
        ${CMAKE_CURRENT_SOURCE_DIR}/check_subset_by_value_interface_method.js
        ${CMAKE_CURRENT_SOURCE_DIR}/check_tuple_type_interface_method.js
        ${CMAKE_CURRENT_SOURCE_DIR}/check_union_type_interface_method.js
        ${CMAKE_CURRENT_SOURCE_DIR}/interface_method.test.js
    COMPILATION_JS_WITH_CJS_ON
)
