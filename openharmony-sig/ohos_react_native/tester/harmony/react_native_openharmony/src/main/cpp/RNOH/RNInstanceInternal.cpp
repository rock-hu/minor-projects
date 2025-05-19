/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
#pragma once
#include <algorithm>
#include <string_view>

#include "RNOH/JSBigStringHelpers.h"
#include "RNOH/RNInstance.h"
#include "TaskExecutor/TaskExecutor.h"

namespace rnoh {

using namespace facebook;

RNInstanceInternal::RNInstanceInternal(
    SharedNativeResourceManager nativeResourceManager)
    : m_nativeResourceManager(std::move(nativeResourceManager))
{
    instance = std::make_shared<facebook::react::Instance>();
    scheduler = nullptr;
}

void RNInstanceInternal::loadScriptFromBuffer(std::vector<uint8_t> bundle,
    std::string const sourceURL,
    std::function<void(const std::string)> onFinish)
{
    this->loadScript(
        JSBigStringHelpers::fromBuffer(std::move(bundle)), sourceURL, onFinish);
}

void RNInstanceInternal::loadScriptFromFile(std::string const fileUrl,
    std::function<void(const std::string)> onFinish)
{
    auto jsBundle = JSBigStringHelpers::fromFilePath(fileUrl);
    if (jsBundle) {
        DLOG(INFO) << "Loaded bundle from file";
    }
    this->loadScript(std::move(jsBundle), fileUrl, onFinish);
}

void RNInstanceInternal::loadScriptFromRawFile(std::string const rawFileUrl,
    std::function<void(const std::string)> onFinish)
{
    // Magic value used to indicate hermes bytecode
    const uint64_t hermesMagic = 0x1F1903C103BC1FC6;

    auto jsBundle = JSBigStringHelpers::fromRawFilePath(
        rawFileUrl, m_nativeResourceManager.get());
    uint64_t extractedMagic{};
    if (jsBundle->size() >= sizeof(uint64_t)) {
        const char* source = jsBundle->c_str();
        std::copy(source,
            source + sizeof(uint64_t),
            reinterpret_cast<uint8_t *>(&extractedMagic));
    }
    if (jsBundle) {
        DLOG(INFO) << "Loaded bundle from rawfile resource";
    }
    if (extractedMagic == hermesMagic) {
        this->loadScript(std::move(jsBundle), rawFileUrl, onFinish);
    } else {
        // NOTE: JS needs to be null terminated to be handled correctly by
        // hermes. Buffers read from a rawfile aren't null terminated, so we
        // pass the buffer as a string.
        std::string s(jsBundle->c_str(), jsBundle->c_str() + jsBundle->size());
        this->loadScript(
            std::make_unique<facebook::react::JSBigStdString>(std::move(s)),
            rawFileUrl,
            onFinish);
    }
}

void RNInstanceInternal::loadScript(
    std::unique_ptr<react::JSBigString const> jsBundle,
    std::string const sourceURL,
    std::function<void(const std::string)> onFinish)
{
    if (m_bundlePath.empty()) {
        m_bundlePath = sourceURL;
    }

    getTaskExecutor()->runTask(TaskThread::JS,
        [weakInstance = std::weak_ptr(instance),
            jsBundle = std::move(jsBundle),
            sourceURL = std::move(sourceURL),
            onFinish = std::move(onFinish)] mutable {
        auto instance = weakInstance.lock();
        if (!instance) {
            // the instance was destroyed before this could run, return an
            // error
            onFinish(
                "The instance was destroyed before the JS bundle could be loaded.");
            return;
        }
        try {
            instance->loadScriptFromString(
                std::move(jsBundle), sourceURL, true);
            onFinish("");
        } catch (std::exception const &e) {
            try {
                std::rethrow_if_nested(e);
                onFinish(e.what());
            } catch (const std::exception &nested) {
                onFinish(e.what() + std::string("\n") + nested.what());
            }
        }
    });
}

void RNInstanceInternal::setBundlePath(std::string const &path)
{
    m_bundlePath = path;
}

std::string RNInstanceInternal::getBundlePath()
{
    return m_bundlePath;
}

NativeResourceManager const *RNInstanceInternal::getNativeResourceManager()
    const
{
    RNOH_ASSERT(m_nativeResourceManager != nullptr);
    return m_nativeResourceManager.get();
}

} // namespace rnoh