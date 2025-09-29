/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * 最佳实践：应用安全编码实践
 * 场景三十二：正确示例，开启属性混淆例子
 */

// [Start code_confusion]
# Define project specific obfuscation rules here.
# You can include the obfuscation configuration files in the current module's build-profile.json5.
#
# For more details, see
#   https://gitee.com/openharmony/arkcompiler_ets_frontend/blob/master/arkguard/README.md
   
# Obfuscation options:
# -disable-obfuscation: disable all obfuscations
# 开启对象属性混淆
-enable-property-obfuscation: obfuscate the property names
# -enable-toplevel-obfuscation: obfuscate the names in the global scope
# -compact: remove unnecessary blank spaces and all line feeds
# -remove-log: remove all console.* statements
# -print-namecache: print the name cache that contains the mapping from the old names to new names
# -apply-namecache: reuse the given cache file

# Keep options:
# -keep-property-name: specifies property names that you want to keep
# -keep-global-name: specifies names that you want to keep in the global scope   
// [End code_confusion]