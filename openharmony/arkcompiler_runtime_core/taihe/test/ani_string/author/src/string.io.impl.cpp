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
#include "taihe/string.hpp"

#include "stdexcept"
#include "string_io.impl.hpp"
// Please delete <stdexcept> include when you implement
using namespace taihe;

namespace {
string input()
{
    return "input";
}

void print(string_view a)
{
    std::cout << a << std::flush;
}

void println(string_view a)
{
    std::cout << a << std::endl;
}
}  // namespace

// because these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_input(input);
TH_EXPORT_CPP_API_print(print);
TH_EXPORT_CPP_API_println(println);
// NOLINTEND
