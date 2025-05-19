/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_COMMON_STREAM_H
#define ARK_COMMON_STREAM_H
#include "common_components/common_runtime/src/base/log_file.h"
namespace panda {
class Stream {
public:
    virtual ~Stream() = default;

    virtual void EndOfStream() = 0;

    // Get chunk's size
    virtual int GetSize() = 0;

    // Writes the chunk of data into the stream
    virtual bool WriteChunk(char* data, int32_t size) = 0;
};
}  // namespace panda

#endif
