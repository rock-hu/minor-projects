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

#include "shortcutInfo.proj.hpp"
#include "shortcutInfo.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"

using namespace taihe;
using namespace shortcutInfo;

namespace {
// To be implemented.

class ShortcutInfoImpl {
public:
    int32_t shortcutInfoImpl = 4096;
    ShortcutInfoImpl() {}

    string GetId()
    {
        return "ShortcutInfo::GetId";
    }

    string GetBundleName()
    {
        return "ShortcutInfo::GetBundleName";
    }

    string GetHostAbility()
    {
        return "ShortcutInfo::GetHostAbility";
    }

    string GetIcon()
    {
        return "ShortcutInfo::GetIcon";
    }

    string GetLabel()
    {
        return "ShortcutInfo::GetLabel";
    }

    int32_t GetLabelId()
    {
        return shortcutInfoImpl;
    }

    int32_t GetIconId()
    {
        return shortcutInfoImpl;
    }

    string GetDisableMessage()
    {
        return "ShortcutInfo::GetDisableMessage";
    }

    optional<bool> GetIsStatic()
    {
        return optional<bool>::make(true);
    }

    optional<bool> GetIsHomeShortcut()
    {
        return optional<bool>::make(true);
    }

    optional<bool> GetIsEnabled()
    {
        return optional<bool>::make(true);
    }
};

ShortcutInfo GetShortcutInfo()
{
    return make_holder<ShortcutInfoImpl, ShortcutInfo>();
}
}  // namespace

TH_EXPORT_CPP_API_GetShortcutInfo(GetShortcutInfo);
