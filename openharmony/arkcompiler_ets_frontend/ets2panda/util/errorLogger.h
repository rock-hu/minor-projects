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

#ifndef ES2PANDA_UTIL_ERROR_LOGGER_H
#define ES2PANDA_UTIL_ERROR_LOGGER_H

#include "es2panda.h"

namespace ark::es2panda::util {

class ErrorLogger {
public:
    explicit ErrorLogger() : ostream_(&std::cout) {}

    std::vector<Error> const &Log() const
    {
        return log_;
    }

    bool IsAnyError()
    {
        return !log_.empty();
    }

    void WriteLog(Error &&error);

    void SetOstream(std::ostream *ostream)
    {
        ostream_ = ostream;
    }

private:
    std::vector<Error> log_;
    std::ostream *ostream_;
};

}  // namespace ark::es2panda::util

#endif
