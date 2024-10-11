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


set(FILE_ITEMS_GEN_INC ${PANDA_BINARY_ROOT}/libpandafile/include/file_items_gen.inc)
panda_gen_file(
    DATA ${GEN_PLUGIN_OPTIONS_YAML}
    TEMPLATE ${PANDA_ROOT}/libpandafile/templates/file_items_gen.inc.erb
    API ${PANDA_ROOT}/templates/plugin_options.rb
    EXTRA_DEPENDENCIES plugin_options_merge
    OUTPUTFILE ${FILE_ITEMS_GEN_INC}
)

set(SOURCE_LANG_ENUM_H ${PANDA_BINARY_ROOT}/libpandafile/include/source_lang_enum.h)
panda_gen_file(
    DATA ${GEN_PLUGIN_OPTIONS_YAML}
    TEMPLATE ${PANDA_ROOT}/libpandafile/templates/source_lang_enum.h.erb
    API ${PANDA_ROOT}/templates/plugin_options.rb
    EXTRA_DEPENDENCIES plugin_options_merge
    OUTPUTFILE ${SOURCE_LANG_ENUM_H}
)

add_custom_target(arkfile_gen DEPENDS
    plugin_options_gen
    ${FILE_ITEMS_GEN_INC}
    ${SOURCE_LANG_ENUM_H}
)

add_dependencies(arkfile arkfile_gen)
add_dependencies(panda_gen_files arkfile_gen)
