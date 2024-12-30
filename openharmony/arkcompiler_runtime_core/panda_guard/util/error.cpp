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

#include "error.h"

#include "utils/logger.h"

std::ostream &panda::guard::Error::GetDescStream()
{
    return descStream_;
}

std::ostream &panda::guard::Error::GetCauseStream()
{
    return causeStream_;
}

std::ostream &panda::guard::Error::GetSolutionsStream()
{
    return solutionsStream_;
}

void panda::guard::Error::Print()
{
    LOG(ERROR, PANDAGUARD) << tag_ << descStream_.str();

    if (errorCode_ == ErrorCode::GENERIC_ERROR) {
        PrintGenericError();  // print to console without displaying internal details
        return;
    }

    std::stringstream ss;
    ss << "[ErrorCode]:" << static_cast<int>(errorCode_) << std::endl;
    ss << "[Description]:" << descStream_.str();
    if (!causeStream_.str().empty()) {
        ss << std::endl;
        ss << "[Cause]:" << causeStream_.str();
    }
    if (!solutionsStream_.str().empty()) {
        ss << std::endl;
        ss << "[Solutions]:" << solutionsStream_.str();
    }
    std::cerr << ss.str() << std::endl;
}

void panda::guard::Error::PrintGenericError()
{
    std::stringstream ss;
    ss << "[ErrorCode]:" << static_cast<int>(ErrorCode::GENERIC_ERROR) << std::endl;
    ss << "[Description]:obfuscate failed" << std::endl;
    std::cerr << ss.str() << std::endl;
}