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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_ETS_NATIVE_LIBRARY_H_
#define PANDA_PLUGINS_ETS_RUNTIME_ETS_NATIVE_LIBRARY_H_

#include "runtime/include/mem/panda_string.h"
#include "libpandabase/os/library_loader.h"

namespace ark::ets {
class EtsNativeLibrary {
public:
    static Expected<EtsNativeLibrary, os::Error> Load(const PandaString &name);

    EtsNativeLibrary(PandaString name, os::library_loader::LibraryHandle &&handle);
    ~EtsNativeLibrary() = default;

    NO_COPY_SEMANTIC(EtsNativeLibrary);
    DEFAULT_MOVE_SEMANTIC(EtsNativeLibrary);

    const PandaString &GetName() const
    {
        return name_;
    }

    Expected<void *, os::Error> FindSymbol(const PandaString &name) const
    {
        return os::library_loader::ResolveSymbol(handle_, name);
    }

    bool operator<(const EtsNativeLibrary &rhs) const
    {
        return handle_.GetNativeHandle() < rhs.handle_.GetNativeHandle();
    }

private:
    PandaString name_;
    os::library_loader::LibraryHandle handle_;
};
}  // namespace ark::ets

#endif  // PANDA_PLUGINS_ETS_RUNTIME_ETS_NATIVE_LIBRARY_H_
