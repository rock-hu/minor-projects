/*
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
#ifndef TAIHE_COMMON_HPP
#define TAIHE_COMMON_HPP

#include <taihe/common.h>

#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

// This file is used as a standard library and needs to be easy to use.
// The length limit of functional macros and internal function name does not apply.
// NOLINTBEGIN

#ifdef __cplusplus
#ifdef __EXCEPTIONS
#define TH_THROW(error_type, message) throw error_type(message)
#else
#define TH_THROW(error_type, message)                                    \
    do {                                                                 \
        fprintf(stderr,                                                  \
                "%s: %s, \nfunction: %s, "                               \
                "file: %s, line %d.\n",                                  \
                #error_type, message, __FUNCTION__, __FILE__, __LINE__); \
        abort();                                                         \
    } while (0)
#endif
#endif

namespace taihe {
template <typename cpp_t, typename = void>
struct as_abi;

template <typename cpp_t>
using as_abi_t = typename as_abi<cpp_t>::type;

template <typename cpp_owner_t, typename = void>
struct as_param;

template <typename cpp_owner_t>
using as_param_t = typename as_param<cpp_owner_t>::type;

template <typename cpp_t>
struct as_abi<cpp_t, std::enable_if_t<std::is_arithmetic_v<cpp_t>>> {
    using type = cpp_t;
};

template <typename cpp_owner_t>
struct as_param<cpp_owner_t, std::enable_if_t<std::is_arithmetic_v<cpp_owner_t>>> {
    using type = cpp_owner_t;
};

template <>
struct as_abi<void> {
    using type = void;
};

template <typename cpp_t, std::enable_if_t<!std::is_reference_v<cpp_t>, int> = 0>
inline as_abi_t<cpp_t> into_abi(cpp_t &&cpp_val)
{
    as_abi_t<cpp_t> abi_val;
    new (&abi_val) cpp_t(std::move(cpp_val));
    return abi_val;
}

template <typename cpp_t, std::enable_if_t<!std::is_reference_v<cpp_t>, int> = 0>
inline as_abi_t<cpp_t> into_abi(cpp_t &cpp_val)
{
    as_abi_t<cpp_t> abi_val;
    new (&abi_val) cpp_t(std::move(cpp_val));
    return abi_val;
}

template <typename cpp_t, std::enable_if_t<!std::is_reference_v<cpp_t>, int> = 0>
inline cpp_t &&from_abi(as_abi_t<cpp_t> &abi_val)
{
    return reinterpret_cast<cpp_t &&>(abi_val);
}

template <typename cpp_t, std::enable_if_t<!std::is_reference_v<cpp_t>, int> = 0>
inline cpp_t &&from_abi(as_abi_t<cpp_t> &&abi_val)
{
    return reinterpret_cast<cpp_t &&>(abi_val);
}

template <typename cpp_t, std::enable_if_t<std::is_reference_v<cpp_t>, int> = 0>
inline as_abi_t<cpp_t> into_abi(cpp_t cpp_val)
{
    return reinterpret_cast<as_abi_t<cpp_t>>(&cpp_val);
}

template <typename cpp_t, std::enable_if_t<std::is_reference_v<cpp_t>, int> = 0>
inline cpp_t from_abi(as_abi_t<cpp_t> abi_val)
{
    return reinterpret_cast<cpp_t>(*abi_val);
}

// enum tags

template <auto tag>
struct static_tag_t {
};

template <auto tag>
constexpr static_tag_t<tag> static_tag = {};

// hash and comparison

struct adl_helper_t {};

template <typename T>
inline std::size_t hash(T &&val)
{
    adl_helper_t adl_helper;
    return hash_impl(adl_helper, std::forward<T>(val));
}

template <typename L, typename R>
inline bool same(L &&lhs, R &&rhs)
{
    adl_helper_t adl_helper;
    return same_impl(adl_helper, std::forward<L>(lhs), std::forward<R>(rhs));
}

template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline std::size_t hash_impl(adl_helper_t, T val)
{
    return std::hash<T> {}(val);
}

template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline bool same_impl(adl_helper_t, T lhs, T rhs)
{
    return lhs == rhs;
}
}  // namespace taihe

// NOLINTEND

#endif // TAIHE_COMMON_HPP
