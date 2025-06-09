/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RAWHEAP_TRANSLATE_COMMON_H
#define RAWHEAP_TRANSLATE_COMMON_H

#include "ecmascript/dfx/hprof/rawheap_translate/utils.h"
#include "ecmascript/dfx/hprof/rawheap_translate/string_hashmap.h"

namespace rawheap_translate {
using JSType = uint8_t;
using NodeType = uint8_t;

static constexpr NodeType DEFAULT_NODETYPE = 8;  // 8: means default node type
static constexpr NodeType FRAMEWORK_NODETYPE = 14;
enum class EdgeType { CONTEXT, ELEMENT, PROPERTY, INTERNAL, HIDDEN, SHORTCUT, WEAK, DEFAULT = PROPERTY };

struct Field {
    std::string name = "";
    uint32_t offset = 0;
    uint32_t size = 0;
};

struct MetaData {
    std::string name = "";
    std::string visitType = "";
    std::vector<Field> fields {};
    MetaData *parent {nullptr};
    uint32_t endOffset = 0;
    JSType type = 0;
    NodeType nodeType = DEFAULT_NODETYPE;

    bool IsArray()
    {
        return visitType == "Array";
    }
};

struct BitField {
    Field objectTypeField;
    Field nativePointerBindingSizeField;
    Field taggedArrayLengthField;
    Field taggedArrayDataField;
    Field dictionaryLengthField;
    Field dictionaryDataField;
};

struct DictionaryLayout {
    uint32_t keyIndex = 0;
    uint32_t valueIndex = 0;
    uint32_t detailIndex = 0;
    uint32_t entrySize = 0;
    uint32_t headerSize = 0;
};

struct TypeRange {
    JSType stringFirst = 0;
    JSType stringLast = 0;
    JSType objectFirst = 0;
    JSType objectLast = 0;
};

struct Node {
    uint64_t nodeId = 0;
    StringId strId = 1;   // 1: for empty string
    uint32_t edgeCount = 0;
    uint32_t index = 0;
    uint32_t size = 0;
    uint32_t nativeSize = 0;
    char *data {nullptr};
    NodeType type = DEFAULT_NODETYPE;
    JSType jsType = 0;

    Node(uint32_t nodeIndex) : index(nodeIndex) {}
};

struct Edge {
    Node *to {nullptr};
    uint32_t nameOrIndex = 0;
    EdgeType type = EdgeType::DEFAULT;

    Edge(Node *node, uint32_t index, EdgeType edgeType) : to(node), nameOrIndex(index), type(edgeType) {}
};
}  // namespace rawheap_translate
#endif  // RAWHEAP_TRANSLATE_COMMON_H