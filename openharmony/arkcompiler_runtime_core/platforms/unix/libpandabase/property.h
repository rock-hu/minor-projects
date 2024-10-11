/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef PLATFORMS_UNIX_LIBPANDABASE_PROPERTY_H
#define PLATFORMS_UNIX_LIBPANDABASE_PROPERTY_H

#include <iosfwd>
#include <string>

namespace panda::os::unix::property {

static const char ARK_DFX_PROP[] = "ark.dfx.options";     // NOLINT(modernize-avoid-c-arrays)
static const char ARK_TRACE_PROP[] = "ark.trace.enable";  // NOLINT(modernize-avoid-c-arrays)

bool GetPropertyBuffer(const char *ark_prop, std::string &out);  // NOLINT(google-runtime-references)

}  // namespace panda::os::unix::property

#endif  // PLATFORMS_UNIX_LIBPANDABASE_PROPERTY_H
