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

#include "literal_data_accessor.h"

namespace panda::panda_file {

LiteralDataAccessor::LiteralDataAccessor(const File &panda_file, File::EntityId literal_data_id)
    : panda_file_(panda_file), literal_data_id_(literal_data_id)
{
    if (ContainsLiteralArrayInHeader(panda_file_.GetHeader()->version)) {
        literal_num_ = panda_file_.GetHeader()->num_literalarrays;
        literal_data_sp_ = panda_file_.GetSpanFromId(literal_data_id_);
    } else {
        literal_num_ = INVALID_INDEX;
    }
}

}  // namespace panda::panda_file