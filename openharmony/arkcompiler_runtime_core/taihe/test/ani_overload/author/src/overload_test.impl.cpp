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
#include "overload_test.impl.hpp"

#include <iostream>

#include "stdexcept"
#include "taihe/string.hpp"
// Please delete <stdexcept> include when you implement
using namespace taihe;

namespace {

class Foo {
public:
    int32_t bar_int(int32_t a)
    {
        return a;
    }

    string bar_str(string_view a)
    {
        return a;
    }
};

int32_t add_int(int32_t a, int32_t b)
{
    return a + b;
}

string add_str(string_view a, string_view b)
{
    return a + b;
}

::overload_test::Foo makeFoo()
{
    return make_holder<Foo, ::overload_test::Foo>();
}

}  // namespace

// because these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_add_int(add_int);
TH_EXPORT_CPP_API_add_str(add_str);
TH_EXPORT_CPP_API_makeFoo(makeFoo);
// NOLINTEND