/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef LIBPANDAFILE_FILE_INL_H
#define LIBPANDAFILE_FILE_INL_H

#include "helpers.h"
#include "file.h"
#include "utils/leb128.h"

namespace panda::panda_file {

inline File::StringData File::GetStringData(EntityId id) const
{
    StringData str_data {};
    auto sp = GetSpanFromId(id);

    auto tag_utf16_length = panda_file::helpers::ReadULeb128(&sp);
    str_data.utf16_length = tag_utf16_length >> 1U;
    str_data.is_ascii = static_cast<bool>(tag_utf16_length & 1U);
    str_data.data = sp.data();

    return str_data;
}

}  // namespace panda::panda_file

#endif  // LIBPANDAFILE_FILE_INL_H
