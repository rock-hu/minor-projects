/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_JSPANDAFILE_ABC_BUFFER_CACHE_H
#define ECMASCRIPT_JSPANDAFILE_ABC_BUFFER_CACHE_H

#include <string>
#include "ecmascript/ecma_vm.h"
#include "ecmascript/js_thread.h"

namespace panda::ecmascript {

enum AbcBufferType { NORMAL_BUFFER, SECURE_BUFFER };
struct AbcBufferInfo {
    void *buffer_ ;
    size_t size_;
    AbcBufferType bufferType_;

    AbcBufferInfo(void *buffer, size_t size, AbcBufferType bufferType)
        : buffer_(buffer), size_(size), bufferType_(bufferType) {}
    AbcBufferInfo()
        : buffer_(nullptr), size_(0), bufferType_(AbcBufferType::NORMAL_BUFFER) {}
};

/*
 * If JSPandafileManager's loadedJSPandaFiles_ delete cache during sharedgc process,
 * buffer cannot get in later module LoadJsPandafile process, which will cause crash.
 * This map can help use buffer to get *pf again.
 */
class AbcBufferCache {
public:
    AbcBufferCache() = default;
    ~AbcBufferCache()
    {
        abcBufferMap_.clear();
    }

    void AddAbcBufferToCache(const CString &fileName, const void *buffer, size_t size, AbcBufferType bufferType)
    {
        abcBufferMap_.emplace(fileName, AbcBufferInfo(const_cast<void *>(buffer), size, bufferType));
    }

    void DeleteAbcBufferFromCache(const CString &fileName)
    {
        auto iter = abcBufferMap_.find(fileName);
        if (iter == abcBufferMap_.end()) {
            return;
        }
        abcBufferMap_.erase(iter);
    }

    AbcBufferInfo FindJSPandaFileInAbcBufferCache(const CString &fileName) const
    {
        auto iter = abcBufferMap_.find(fileName);
        if (iter == abcBufferMap_.end()) {
            return AbcBufferInfo();
        }
        return iter->second;
    }
    
private:
    std::unordered_map<CString, AbcBufferInfo> abcBufferMap_;
};

class AbcBufferCacheScope {
public:
    AbcBufferCacheScope(JSThread *thread, const CString &filename, const void *buffer,
        size_t size, AbcBufferType bufferType): filename_(filename)
    {
        abcBufferCache_ = thread->GetEcmaVM()->GetAbcBufferCache();
        ASSERT(abcBufferCache_ != nullptr);
        abcBufferCache_->AddAbcBufferToCache(filename_, buffer, size, bufferType);
    }

    AbcBufferCacheScope(JSThread *thread, const CString &filename, const void *buffer, size_t size,
        JSPandaFile *jsPandaFile, void *fileMapper)
        : filename_(filename), jsPandaFile_(jsPandaFile), fileMapper_(fileMapper)
    {
        abcBufferCache_ = thread->GetEcmaVM()->GetAbcBufferCache();
        ASSERT(abcBufferCache_ != nullptr);
        // if input has fileMapper, assume it's secure memory
        abcBufferCache_->AddAbcBufferToCache(filename_, buffer, size, AbcBufferType::SECURE_BUFFER);
    }

    ~AbcBufferCacheScope()
    {
        ASSERT(abcBufferCache_ != nullptr);
        abcBufferCache_->DeleteAbcBufferFromCache(filename_);
        // secure memory & make sure buffer is not used in pandafile
        if (fileMapper_ != nullptr && jsPandaFile_->GetFileMapper() != fileMapper_) {
            // release secure memory buffer
            JSPandaFile::CallReleaseSecureMemFunc(fileMapper_);
        }
    }

private:
    const CString filename_;
    JSPandaFile *jsPandaFile_ {nullptr};
    void *fileMapper_ {nullptr};
    AbcBufferCache *abcBufferCache_ {nullptr};
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_JSPANDAFILE_ABC_BUFFER_CACHE_H