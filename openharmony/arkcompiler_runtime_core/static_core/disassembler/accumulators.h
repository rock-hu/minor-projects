/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISASM_ACCUMULATORS_H_INCLUDED
#define DISASM_ACCUMULATORS_H_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "libpandafile/debug_info_extractor.h"

namespace ark::disasm {
using LabelTable = std::map<size_t, std::string>;
using IdList = std::vector<ark::panda_file::File::EntityId>;

struct MethodInfo {
    std::string methodInfo;

    std::vector<std::string> instructionsInfo;

    panda_file::LineNumberTable lineNumberTable;

    panda_file::LocalVariableTable localVariableTable;
};

struct RecordInfo {
    std::string recordInfo;

    std::vector<std::string> fieldsInfo;
};

struct ProgInfo {
    std::map<std::string, RecordInfo> recordsInfo;
    std::map<std::string, MethodInfo> methodsInfo;
};

using AnnotationList = std::vector<std::pair<std::string, std::string>>;

struct RecordAnnotations {
    AnnotationList annList;
    std::map<std::string, AnnotationList> fieldAnnotations;
};

struct ProgAnnotations {
    std::map<std::string, AnnotationList> methodAnnotations;
    std::map<std::string, RecordAnnotations> recordAnnotations;
};
}  // namespace ark::disasm

#endif
