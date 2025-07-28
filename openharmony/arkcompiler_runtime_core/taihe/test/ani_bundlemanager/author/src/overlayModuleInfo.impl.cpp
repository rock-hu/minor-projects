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
#include "overlayModuleInfo.proj.hpp"
#include "overlayModuleInfo.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"

using namespace taihe;
using namespace overlayModuleInfo;

namespace {
// To be implemented.

class OverlayModuleInfoImpl {
public:
    int32_t overlayModuleInfoImpl = 21474;
    OverlayModuleInfoImpl() {}

    string GetBundleName()
    {
        return "OverlayModuleInfoImpl::getBundleName";
    }

    string GetModuleName()
    {
        return "OverlayModuleInfoImpl::getModuleName";
    }

    string GetTargetModuleName()
    {
        return "OverlayModuleInfoImpl::getTargetModuleName";
    }

    int32_t GetPriority()
    {
        return overlayModuleInfoImpl;
    }

    int32_t GetState()
    {
        return overlayModuleInfoImpl;
    }
};

OverlayModuleInfo GetOverlayModuleInfo()
{
    return make_holder<OverlayModuleInfoImpl, OverlayModuleInfo>();
}
}  // namespace

TH_EXPORT_CPP_API_GetOverlayModuleInfo(GetOverlayModuleInfo);