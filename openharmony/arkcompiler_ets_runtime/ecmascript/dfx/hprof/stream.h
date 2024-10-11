/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_DFX_HPROF_STREAM_H
#define ECMASCRIPT_DFX_HPROF_STREAM_H

namespace panda::ecmascript {
class HeapStat {
public:
    HeapStat(int32_t index, int32_t count, int32_t size)
        : index_(index), count_(count), size_(size) {}
    ~HeapStat() = default;

    int32_t index_ {0};
    int32_t count_ {0};
    int32_t size_ {0};
};

class Stream {
public:
    virtual ~Stream() = default;

    virtual void EndOfStream() = 0;

    // Get chunk's size
    virtual int GetSize() = 0;

    // Writes the chunk of data into the stream
    virtual bool WriteChunk(char *data, int32_t size) = 0;
    virtual bool WriteBinBlock(char *data, int32_t size) = 0;
    virtual bool Good() = 0;
    virtual void UpdateHeapStats(HeapStat* data, int32_t count) = 0;
    virtual void UpdateLastSeenObjectId(int32_t lastSeenObjectId, int64_t timeStampUs) = 0;
};
}  // namespace panda::ecmascript

#endif  // ECMASCRIPT_DFX_HPROF_STREAM_H
