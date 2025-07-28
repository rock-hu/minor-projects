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
#include "nova.impl.hpp"
#include "nova.proj.hpp"
#include "stdexcept"
#include "taihe/runtime.hpp"

using namespace taihe;
using namespace nova;

namespace {
// To be implemented.

class NovaTypeImpl {
public:
    NovaTypeImpl()
    {
        // Don't forget to implement the constructor.
    }
};

void testBar(::mate::bar::BarType const &bar)
{
    TH_THROW(std::runtime_error, "testBar not implemented");
}

void testPura(::pura::PuraType pura)
{
    TH_THROW(std::runtime_error, "testPura not implemented");
}

void testNova(weak::NovaType nova)
{
    TH_THROW(std::runtime_error, "testNova not implemented");
}
}  // namespace

// because these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_testBar(testBar);
TH_EXPORT_CPP_API_testPura(testPura);
TH_EXPORT_CPP_API_testNova(testNova);
// NOLINTEND