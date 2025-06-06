/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_SYNTAX_IF_ELSE_MODEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_SYNTAX_IF_ELSE_MODEL_H

#include <cstdint>
#include <memory>
#include <list>
#include <mutex>

#include "base/utils/macros.h"

namespace OHOS::Ace {

class ACE_FORCE_EXPORT IfElseModel {
public:
    static IfElseModel* GetInstance();

    virtual ~IfElseModel() = default;
    virtual void Create() = 0;
    virtual void Pop() = 0;
    virtual void SetBranchId(int32_t value, std::list<int32_t>& removedElmtId, std::list<int32_t>& reservedElmtId) = 0;
    virtual int32_t GetBranchId() = 0;
    virtual bool CanRetake(const std::string& id) = 0;
    virtual bool GetRetakenElmtIds(std::list<int32_t>& retakenElmtIds) = 0;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_SYNTAX_IF_ELSE_MODEL_H
