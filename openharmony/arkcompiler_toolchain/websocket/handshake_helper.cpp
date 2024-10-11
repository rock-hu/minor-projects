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

#include "common/log_wrapper.h"
#include "handshake_helper.h"

namespace OHOS::ArkCompiler::Toolchain {
/* static */
bool WebSocketKeyEncoder::EncodeKey(std::string_view key, unsigned char (&destination)[ENCODED_KEY_LEN + 1])
{
    std::string buffer(key.size() + WEB_SOCKET_GUID.size(), 0);
    key.copy(buffer.data(), key.size());
    WEB_SOCKET_GUID.copy(buffer.data() + key.size(), WEB_SOCKET_GUID.size());

    return EncodeKey(reinterpret_cast<unsigned char *>(buffer.data()), buffer.size(), destination);
}

/* static */
bool WebSocketKeyEncoder::EncodeKey(const unsigned char(&key)[KEY_LENGTH + 1],
                                    unsigned char (&destination)[ENCODED_KEY_LEN + 1])
{
    constexpr size_t bufferSize = KEY_LENGTH + WEB_SOCKET_GUID.size();
    unsigned char buffer[bufferSize];
    auto *guid = std::copy(key, key + KEY_LENGTH, buffer);
    WEB_SOCKET_GUID.copy(reinterpret_cast<char *>(guid), WEB_SOCKET_GUID.size());

    return EncodeKey(buffer, bufferSize, destination);
}

/* static */
bool WebSocketKeyEncoder::EncodeKey(const unsigned char *source, size_t length,
                                    unsigned char (&destination)[ENCODED_KEY_LEN + 1])
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(source, length, hash);

    // base64-encoding is done via EVP_EncodeBlock, which writes a null-terminated string.
    int encodedBytes = EVP_EncodeBlock(destination, hash, SHA_DIGEST_LENGTH);
    // "EVP_EncodeBlock() returns the number of bytes encoded excluding the NUL terminator."
    if (encodedBytes != ENCODED_KEY_LEN) {
        LOGE("EVP_EncodeBlock failed to encode all bytes, encodedBytes = %{public}d", encodedBytes);
        return false;
    }
    return true;
}
} // namespace OHOS::ArkCompiler::Toolchain
