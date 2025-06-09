/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

export enum SubsystemCode {
  BUILDSYSTEM = '114',
  ES2PANDA = '115',
}

export enum ErrorCode {
  BUILDSYSTEM_COMPILE_ABC_FAIL = '11410001',
  BUILDSYSTEM_LINK_ABC_FAIL = '11410002',
  BUILDSYSTEM_SOURCEROOTS_NOT_SET_FAIL = '11410003',
  BUILDSYSTEM_UNRECOGNIZED_MODULEROOTPATH = '11410004',
  BUILDSYSTEM_MODULE_INFO_NOT_CORRECT_FAIL = '11410005',
  BUILDSYSTEM_DEPENDENT_MODULE_INFO_NOT_CORRECT_FAIL = '11410006',
  BUILDSYSTEM_FILE_NOT_BELONG_TO_ANY_MODULE_FAIL = '11410007',
  BUILDSYSTEM_LOAD_PLUGIN_FAIL = '11410008',
  BUILDSYSTEM_ARK_LINK_NOT_FOUND_FAIL = '11410009',
  BUILDSYSTEM_SDK_NOT_EXIST_FAIL = '11410010',
  BUILDSYSTEM_DEPENDENT_MODULE_INFO_NOT_FOUND = '11410011',
  BUILDSYSTEM_PLUGIN_CONTEXT_RESET_PROJECT_CONFIG = '11410012',
  BUILDSYSTEM_DECLGEN_FAIL = '11410013'
}
