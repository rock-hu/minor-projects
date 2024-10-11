/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_VERIFIER_DEBUG_MSG_SET_PARSER_H_
#define PANDA_VERIFIER_DEBUG_MSG_SET_PARSER_H_

#include "runtime/include/mem/panda_containers.h"
#include "verification/util/parser/parser.h"
#include "verifier_messages.h"

namespace ark::verifier::debug {

struct MessageSetContext {
    ark::PandaVector<std::pair<size_t, size_t>> stack;
    ark::PandaUnorderedSet<size_t> nums;
};

}  // namespace ark::verifier::debug

#endif  // PANDA_VERIFIER_DEBUG_MSG_SET_PARSER_H_
