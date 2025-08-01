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
#include "iface_readonly_test.impl.hpp"

#include "iface_readonly_test.Noo.proj.2.hpp"
#include "stdexcept"
#include "taihe/string.hpp"
// Please delete <stdexcept> include when you implement
using namespace taihe;

namespace {
class Noo {
    string name_ {"noo"};

public:
    void bar()
    {
        std::cout << "Nooimpl: " << __func__ << std::endl;
    }

    string getName()
    {
        std::cout << "Nooimpl: " << __func__ << " " << name_ << std::endl;
        return name_;
    }
};

::iface_readonly_test::Noo getNooIface()
{
    return make_holder<Noo, ::iface_readonly_test::Noo>();
}

string printNooName(::iface_readonly_test::weak::Noo noo)
{
    auto name = noo->getName();
    std::cout << __func__ << ": " << name << std::endl;
    return name;
}
}  // namespace

// because these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_getNooIface(getNooIface);
TH_EXPORT_CPP_API_printNooName(printNooName);
// NOLINTEND