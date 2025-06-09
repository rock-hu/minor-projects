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
#include <iostream>
#include <maythrow.impl.hpp>

#include "taihe/runtime.hpp"

namespace {
int32_t maythrow_impl(int32_t a)
{
    if (a == 0) {
        taihe::set_error("some error happen");
        return -1;
    } else {
        int tempnum = 10;
        return a + tempnum;
    }
}

maythrow::Data getDataMaythrow()
{
    taihe::set_error("error in getDataMaythrow");
    return {
        taihe::string("C++ Object"),
        (float)1.0,
        {"data.obj", "file.txt"},
    };
}

void noReturnMaythrow()
{
    taihe::set_error("error in noReturnMaythrow");
}

void noReturnBusinessError()
{
    int errorcode = 5;
    taihe::set_business_error(errorcode, "error in noReturnBusinessError");
}
}  // namespace
// The macros used below are automatically generated code.
// NOLINTBEGIN
TH_EXPORT_CPP_API_maythrow(maythrow_impl);
TH_EXPORT_CPP_API_getDataMaythrow(getDataMaythrow);
TH_EXPORT_CPP_API_noReturnMaythrow(noReturnMaythrow);
TH_EXPORT_CPP_API_noReturnBusinessError(noReturnBusinessError);
// NOLINTEND
