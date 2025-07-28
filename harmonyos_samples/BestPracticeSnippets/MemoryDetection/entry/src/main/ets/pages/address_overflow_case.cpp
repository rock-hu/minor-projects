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

/**
 * 最佳实践：地址越界问题案例
 */

// [Start address_overflow_case_1]
template<class T, class O>
static RefPtr<T> DynamicCast(const RefPtr<O>& ptr)
{
    return Claim(DynamicCast<T>(RawPtr(ptr))); // 崩溃栈顶
}
// [End address_overflow_case_1]

// [Start address_overflow_case_2]
RefPtr<NG::FrameNode> GetFramenodeByAccessibilityId(const RefPtr<NG::FrameNode>& root, int64_t id)
{
    CHECK_NULL_RETURN(root, nullptr);
    if (root->GetAccessibilityId() == id) {
        return root;
    }
    std::queue<RefPtr<NG::UINode>> nodes;
    nodes.push(root);
    RefPtr<NG::FrameNode> frameNodeResult = nullptr;

    while (!nodes.empty()) {
        auto current = nodes.front();
        nodes.pop();
        if (current->HasVirtualNodeAccessibilityProperty()) {
            auto fnode = AceType::DynamicCast<NG::FrameNode>(current);
            auto property = fnode->GetAccessibilityProperty<NG::AccessibilityProperty>();
            const auto& children = std::list<RefPtr<NG::UINode>> { property->GetAccessibilityVirtualNode() };
            if (FindFrameNodeByAccessibilityId(id, children, nodes, frameNodeResult)) {
                return frameNodeResult;
            }
        } else {
            const auto& children = current->GetChildren(true);
            if (FindFrameNodeByAccessibilityId(id, children, nodes, frameNodeResult)) {
                return frameNodeResult;
            }
        }
        auto frameNode = AceType::DynamicCast<NG::FrameNode>(current);
        if (!frameNode) {
            continue;
        }
        auto overlayNode = frameNode->GetOverlayNode();
        if (overlayNode) {
            const auto& children = std::list<RefPtr<NG::UINode>> { overlayNode };
            if (FindFrameNodeByAccessibilityId(id, children, nodes, frameNodeResult)) {
                return frameNodeResult;
            }
        }
    }
    return nullptr;
}
// [End address_overflow_case_2]

// [Start address_overflow_case_3]
template<typename T>
RefPtr<T> GetAccessibilityProperty() const
{
    return DynamicCast<T>(accessibilityProperty_);
}
// [End address_overflow_case_3]

// [Start address_overflow_case_4]
errno_t memcpy_s(void *dest, size_t destMax, const void *src, size_t count) // dest 对象存在问题
{
    if (SECUREC_MEMCPY_PARAM_OK(dest, destMax, src, count)) {
        SECUREC_MEMCPY_WARP_OPT(dest, src, count);
        return EOK;
    }
    /* Meet some runtime violation, return error code */
    return SecMemcpyError(dest, destMax, src, count);
}
// [End address_overflow_case_4]

// [Start address_overflow_case_5]
void CacheBuffer::DealWriteData(size_t length)
{
    CHECK_AND_RETURN_LOG(audioRenderer_ != nullptr, "DealWriteData audioRenderer_ is nullptr");
    AudioStandard::BufferDesc bufDesc;
    audioRenderer_->GetBufferDesc(bufDesc);
    if (bufDesc.buffer != nullptr && fullCacheData_ != nullptr && fullCacheData_->buffer != nullptr) {
        if (static_cast<size_t>(fullCacheData_->size) - cacheDataFrameIndex_ >= length) {
            int32_t ret = memcpy_s(bufDesc.buffer, length,
                fullCacheData_->buffer + cacheDataFrameIndex_, length); // 问题点
            CHECK_AND_RETURN_LOG(ret == MSERR_OK, "memcpy failed total length.");
            bufDesc.bufLength = length;
            bufDesc.dataLength = length;
            cacheDataFrameIndex_ += length;
        } else {
            size_t copyLength = static_cast<size_t>(fullCacheData_->size) - cacheDataFrameIndex_;
            int32_t ret = AudioStandard::AudioRenderer::MuteAudioBuffer(bufDesc.buffer, 0, length, sampleFormat_);
            CHECK_AND_RETURN_LOG(ret == AudioStandard::SUCCESS, "fill mute buffer failed.");
            ret = memcpy_s(bufDesc.buffer, length, fullCacheData_->buffer + cacheDataFrameIndex_,
                copyLength);
            CHECK_AND_RETURN_LOG(ret == MSERR_OK, "memcpy failed not enough length.");
            bufDesc.bufLength = length;
            bufDesc.dataLength = length;
            cacheDataFrameIndex_ += copyLength;
        }
        FadeInAudioBuffer(bufDesc);
        audioRenderer_->Enqueue(bufDesc);
    } else {
        MEDIA_LOGE("OnWriteData, cacheDataFrameIndex_: %{public}zu, length: %{public}zu,"
            " bufDesc.buffer:%{public}d, fullCacheData_:%{public}d, fullCacheData_->buffer:%{public}d,"
            " soundID_:%{public}d, streamID_:%{public}d",
            cacheDataFrameIndex_, length, bufDesc.buffer != nullptr, fullCacheData_ != nullptr,
            fullCacheData_->buffer != nullptr, soundID_, streamID_);
    }
}
// [End address_overflow_case_5]

// [Start address_overflow_case_6]
RendererInClientInner::~RendererInClientInner()
{
    AUDIO_INFO_LOG("~RendererInClientInner()");
    DumpFileUtil::CloseDumpFile(&dumpOutFd_);
    RendererInClientInner::ReleaseAudioStream(true);
    std::lock_guard<std::mutex> runnerlock(runnerMutex_);
    if (!runnerReleased_ && callbackHandler_ != nullptr) {
        AUDIO_INFO_LOG("runner remove");
        callbackHandler_->ReleaseEventRunner();
        runnerReleased_ = true;
        callbackHandler_ = nullptr;
    }
    UnregisterSpatializationStateEventListener(spatializationRegisteredSessionID_);
    AUDIO_INFO_LOG("[%{public}s] volume data counts: %{public}" PRId64, logUtilsTag_.c_str(), volumeDataCount_);
}
// [End address_overflow_case_6]

// [Start address_overflow_case_7]
int32_t RendererInClientInner::GetBufferDesc(BufferDesc &bufDesc)
{
    Trace trace("RendererInClientInner::GetBufferDesc");
    if (renderMode_ != RENDER_MODE_CALLBACK) {
        AUDIO_ERR_LOG("GetBufferDesc is not supported. Render mode is not callback.");
        return ERR_INCORRECT_MODE;
    }
    std::lock_guard<std::mutex> lock(cbBufferMutex_);
    bufDesc.buffer = cbBuffer_.get();
    bufDesc.bufLength = cbBufferSize_;
    bufDesc.dataLength = cbBufferSize_;
    if (curStreamParams_.encoding == ENCODING_AUDIOVIVID) {
        CHECK_AND_RETURN_RET_LOG(converter_ != nullptr, ERR_INVALID_OPERATION, "converter is not inited");
        bufDesc.metaBuffer = bufDesc.buffer + cbBufferSize_;
        bufDesc.metaLength = converter_->GetMetaSize();
    }
    return SUCCESS;
}
// [End address_overflow_case_7]