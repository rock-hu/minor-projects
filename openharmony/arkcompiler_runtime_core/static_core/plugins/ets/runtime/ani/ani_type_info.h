/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_ANI_ANI_TYPE_INFO_H
#define PANDA_PLUGINS_ETS_RUNTIME_ANI_ANI_TYPE_INFO_H

#include "plugins/ets/runtime/ani/ani.h"
#include "plugins/ets/runtime/types/ets_type.h"

namespace ark::ets::ani {

template <typename T>
struct AniTypeInfo;

template <>
struct AniTypeInfo<ani_boolean> {
    static constexpr EtsType ETS_TYPE_VALUE = EtsType::BOOLEAN;
};

template <>
struct AniTypeInfo<ani_char> {
    static constexpr EtsType ETS_TYPE_VALUE = EtsType::CHAR;
};

template <>
struct AniTypeInfo<ani_byte> {
    static constexpr EtsType ETS_TYPE_VALUE = EtsType::BYTE;
};

template <>
struct AniTypeInfo<ani_short> {
    static constexpr EtsType ETS_TYPE_VALUE = EtsType::SHORT;
};

template <>
struct AniTypeInfo<ani_int> {
    static constexpr EtsType ETS_TYPE_VALUE = EtsType::INT;
};

template <>
struct AniTypeInfo<ani_long> {
    static constexpr EtsType ETS_TYPE_VALUE = EtsType::LONG;
};

template <>
struct AniTypeInfo<ani_float> {
    static constexpr EtsType ETS_TYPE_VALUE = EtsType::FLOAT;
};

template <>
struct AniTypeInfo<ani_double> {
    static constexpr EtsType ETS_TYPE_VALUE = EtsType::DOUBLE;
};

template <>
struct AniTypeInfo<ani_ref> {
    static constexpr EtsType ETS_TYPE_VALUE = EtsType::OBJECT;
};

}  // namespace ark::ets::ani

#endif  // PANDA_PLUGINS_ETS_RUNTIME_ANI_ANI_TYPE_INFO_H
