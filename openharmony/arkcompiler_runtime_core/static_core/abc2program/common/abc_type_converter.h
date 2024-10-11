/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ABC2PROGRAM_COMMON_ABC_TYPE_CONVERTER_H
#define ABC2PROGRAM_COMMON_ABC_TYPE_CONVERTER_H

#include <assembly-ins.h>
#include "proto_data_accessor-inl.h"
#include "abc_string_table.h"

namespace ark::abc2program {

class AbcTypeConverter {
public:
    explicit AbcTypeConverter(AbcStringTable &stringTable) : stringTable_(stringTable) {}
    pandasm::Type PandaFileTypeToPandasmType(const panda_file::Type &type, panda_file::ProtoDataAccessor &pda,
                                             size_t &refIdx) const;
    pandasm::Type FieldTypeToPandasmType(const uint32_t &type) const;

private:
    AbcStringTable &stringTable_;
};  // class AbcTypeConverter

}  // namespace ark::abc2program

#endif  // ABC2PROGRAM_COMMON_ABC_TYPE_CONVERTER_H