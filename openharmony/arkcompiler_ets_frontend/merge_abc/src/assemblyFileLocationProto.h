/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef MERGE_ABC_ASSEMBLY_FILE_LOCATION_H
#define MERGE_ABC_ASSEMBLY_FILE_LOCATION_H

#include "assemblyFileLocation.pb.h"
#include "assembly-program.h"

namespace panda::proto {
class FileLocation {
public:
    static void Serialize(const panda::pandasm::FileLocation &location, protoPanda::FileLocation &protoLocation);
    static void Deserialize(const protoPanda::FileLocation &protoLocation,
                            std::optional<panda::pandasm::FileLocation> &location);
};
} // panda::proto
#endif
