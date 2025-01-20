/*
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

#include "ecmascript/dfx/hprof/file_stream.h"

#include <climits>
#include <unistd.h>

#include "ecmascript/log_wrapper.h"

namespace panda::ecmascript {
FileStream::FileStream(const std::string &fileName)
{
    Initialize(fileName);
}

FileStream::~FileStream()
{
    EndOfStream();
}

void FileStream::EndOfStream()
{
    if (fileStream_.is_open()) {
        fileStream_.close();
    }
}

bool FileStream::Good()
{
    return fileStream_.good();
}

void FileStream::Initialize(const std::string &fileName)
{
    // check file name
    std::pair<bool, std::string> realPath = FilePathValid(fileName);
    if (!realPath.first) {
        LOG_ECMA(ERROR) << "FileStream: check file path failed";
        return;
    }

    fileStream_.open(realPath.second.c_str(), std::ios::out);
    if (fileStream_.fail()) {
        LOG_ECMA(ERROR) << "FileStream: open file failed";
    }
}

std::pair<bool, std::string> FileStream::FilePathValid(const std::string &fileName)
{
    if (fileName.empty() || fileName.size() > PATH_MAX) {
        return std::make_pair(false, "");
    }
    char resolvedPath[PATH_MAX] = {0};
    auto result = realpath(fileName.c_str(), resolvedPath);
    if (result == resolvedPath || errno == ENOENT) {
        return std::make_pair(true, std::string(resolvedPath));
    }
    return std::make_pair(false, "");
}

// Writes the chunk of data into the stream
bool FileStream::WriteChunk(char *data, int32_t size)
{
    if (fileStream_.fail()) {
        return false;
    }

    std::string str(data, size);

    fileStream_ << str;

    return true;
}

void FileDescriptorStream::EndOfStream()
{
    if (Good()) {
        close(fd_);
    }
}

bool FileDescriptorStream::Good()
{
    return fd_ > 0;
}

// Writes the chunk of data into the stream
bool FileDescriptorStream::WriteChunk(char *data, int32_t size)
{
    if (fd_ < 0) {
        return false;
    }

    std::string str(data, size);
    int ret = dprintf(fd_, "%s", str.c_str());
    if (ret < 0) {
        LOG_ECMA(ERROR) << "Write FD print failed, ret" << ret;
        return false;
    }
    ret = fsync(fd_);
    if (ret < 0) {
        LOG_ECMA(ERROR) << "Write FD file failed, ret" << ret;
        return false;
    }
    return true;
}

bool FileDescriptorStream::WriteBinBlock(char *data, int32_t size)
{
    if (fd_ < 0) {
        return false;
    }
    ssize_t written = write(fd_, data, size);
    if (written == -1) {
        perror("write");
        return false;
    }
    return true;
}
}
