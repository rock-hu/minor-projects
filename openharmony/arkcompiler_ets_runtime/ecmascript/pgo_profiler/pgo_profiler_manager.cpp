/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ecmascript/pgo_profiler/pgo_profiler_manager.h"

#include "ecmascript/platform/file.h"
#include "ecmascript/platform/os.h"

namespace panda::ecmascript::pgo {
namespace {
constexpr int32_t PGO_SAVING_SIGNAL = 50;
} // namespace

PGOProfilerManager* PGOProfilerManager::GetInstance()
{
    static PGOProfilerManager* instance = new PGOProfilerManager();
    return instance;
}

bool PGOProfilerManager::MergeApFiles(const std::string& inFiles,
                                      const std::string& outPath,
                                      uint32_t hotnessThreshold,
                                      ApGenMode mode)
{
    std::string realPath;
    if (!PGOProfilerManager::ResetOutPath(outPath, realPath, ApNameUtils::DEFAULT_AP_NAME)) {
        LOG_ECMA(ERROR) << "reset out path failed, outPath: " << outPath
                        << " ,hotnessThreshold: " << hotnessThreshold;
        return false;
    }
    arg_list_t apFileNames = base::StringHelper::SplitString(inFiles, GetFileDelimiter());
    auto info = std::make_shared<PGOInfo>(hotnessThreshold);
    bool hasMerged = false;
    std::string firstApFileName;
    for (const auto& fileName: apFileNames) {
        if (!base::StringHelper::EndsWith(fileName, ".ap")) {
            LOG_ECMA(ERROR) << "The file path (" << fileName << ") does not end with .ap";
            continue;
        }
        PGOProfilerDecoder decoder(fileName, hotnessThreshold);
        if (!decoder.LoadFull(info->GetAbcFilePoolPtr())) {
            LOG_ECMA(ERROR) << "Fail to load file path (" << fileName << "), skip it.";
            continue;
        }
        if (!hasMerged) {
            firstApFileName = fileName;
        } else {
            if (!info->VerifyPandaFileMatched(decoder.GetPandaFileInfos(), firstApFileName, fileName)) {
                continue;
            }
        }
        if (!decoder.IsCompatibleWithAOTFile()) {
            LOG_ECMA(ERROR) << "The ap file (" << fileName << ") is not compatible with AOT version. skip it";
            continue;
        }
        info->MergeSafe(decoder.GetRecordDetailInfos());
        info->MergeSafe(decoder.GetPandaFileInfos());
        hasMerged = true;
    }
    if (!hasMerged) {
        LOG_ECMA(ERROR)
            << "No ap file pass verify, no ap file compatible an version, no ap file processed. Input files: "
            << inFiles;
        GetInstance()->SetIsApFileCompatible(false);
        return false;
    }
    GetInstance()->SetIsApFileCompatible(true);
    PGOProfilerEncoder encoder(outPath, mode);
    encoder.Save(info);
    return true;
}

bool PGOProfilerManager::MergeApFiles(std::unordered_map<CString, uint32_t> &fileNameToChecksumMap,
                                      PGOProfilerDecoder &merger)
{
    uint32_t hotnessThreshold = merger.GetHotnessThreshold();
    std::string inFiles(merger.GetInPath());
    arg_list_t pgoFileNamesVector = base::StringHelper::SplitString(inFiles, GetFileDelimiter());
    if (pgoFileNamesVector.empty()) {
        return true;
    }
    merger.InitMergeData();
    bool hasMerged = false;
    std::string firstApFileName;
    for (const auto& fileName : pgoFileNamesVector) {
        PGOProfilerDecoder decoder(fileName, hotnessThreshold);
        if (!decoder.LoadAndVerify(fileNameToChecksumMap, merger.GetAbcFilePool())) {
            LOG_ECMA(ERROR) << "Load and verify file (" << fileName << ") failed, skip it.";
            continue;
        }
        if (!hasMerged) {
            firstApFileName = fileName;
        } else {
            if (!merger.GetPandaFileInfos().VerifyChecksum(decoder.GetPandaFileInfos(), firstApFileName, fileName)) {
                continue;
            }
        }
        if (!decoder.IsCompatibleWithAOTFile()) {
            LOG_ECMA(ERROR) << "The ap file (" << fileName << ") is not compatible with AOT version. skip it";
            continue;
        }
        merger.Merge(decoder);
        hasMerged = true;
    }
    if (!hasMerged) {
        LOG_ECMA(ERROR)
            << "No ap file pass verify, no ap file compatible an version, no ap file processed. Input files: "
            << inFiles;
        GetInstance()->SetIsApFileCompatible(false);
        return false;
    }
    GetInstance()->SetIsApFileCompatible(true);
    // In the scenario of on-device application compilation, the input fileNameToChecksumMap only contains ABC
    // information of the application, while AP holds all ABC information collected during the
    // collection phase. Considering external HSP dependencies, the input fileNameToChecksumMap
    // needs to be merged with the information from AP, that we can write full ABC information to the an file.
    merger.MergeFileNameToChecksumMap(fileNameToChecksumMap);
    return true;
}

void PGOProfilerManager::RegisterSavingSignal()
{
    LOG_PGO(INFO) << "register PGO force save signal";
    if (!isInitialized_) {
        LOG_PGO(ERROR) << "can not register pgo saving signal, data is not initialized";
        return;
    }
    signal(PGO_SAVING_SIGNAL, SavingSignalHandler);
    enableSignalSaving_ = true;
}

void PGOProfilerManager::SavingSignalHandler(int signo)
{
    if (signo != PGO_SAVING_SIGNAL) {
        return;
    }

    PGOProfilerManager::GetInstance()->ForceDump();
}

void PGOProfilerManager::Initialize(const std::string& outDir, uint32_t hotnessThreshold)
{
    outDir_ = outDir;
    hotnessThreshold_ = hotnessThreshold;
    pgoInfo_ = std::make_shared<PGOInfo>(hotnessThreshold);
    state_ = std::make_shared<PGOState>();
    LOG_PGO(INFO) << "pgo profiler manager initialized, output directory: " << outDir
                  << ", hotness threshold: " << hotnessThreshold
                  << ", state: " << state_->ToString(state_->GetState());
}

void PGOProfilerManager::SetBundleName(const std::string& bundleName)
{
    bundleName_ = bundleName;
}

const std::string PGOProfilerManager::GetBundleName() const
{
    return bundleName_;
}

void PGOProfilerManager::SetRequestAotCallback(const RequestAotCallback& cb)
{
    LockHolder lock(requestAotCallbackMutex_);
    if (requestAotCallback_ != nullptr) {
        return;
    }
    requestAotCallback_ = cb;
}

bool PGOProfilerManager::RequestAot(const std::string& bundleName,
                                    const std::string& moduleName,
                                    RequestAotMode triggerMode)
{
    RequestAotCallback cb;
    {
        LockHolder lock(requestAotCallbackMutex_);
        if (requestAotCallback_ == nullptr) {
            LOG_PGO(ERROR) << "trigger aot failed, callback is null";
            return false;
        }
        cb = requestAotCallback_;
    }
    return (cb(bundleName, moduleName, static_cast<int32_t>(triggerMode)) == 0);
}

void PGOProfilerManager::Destroy()
{
    LOG_PGO(INFO) << "attempting to destroy pgo profiler manager, pgo profiler data initialized: "
                  << isInitialized_;
    if (isInitialized_) {
        PGOProfilerEncoder encoder(outPath_, apGenMode_);
        {
            LockHolder lock(GetPGOInfoMutex());
            encoder.Save(pgoInfo_);
            pgoInfo_->Clear();
            pgoInfo_.reset();
        }
        state_.reset();
        isInitialized_ = false;
        apGenMode_ = ApGenMode::MERGE;
        outPath_ = "";
        LOG_PGO(INFO) << "pgo profiler manager destroied";
    }
}

std::shared_ptr<PGOProfiler> PGOProfilerManager::BuildProfiler(EcmaVM* vm, bool isEnable)
{
    LOG_PGO(INFO) << "build profiler, pgo enable: " << isEnable;
    if (isEnable) {
        isEnable = InitializeData();
    }
    auto profiler = std::make_shared<PGOProfiler>(vm, isEnable);
    {
        LockHolder lock(profilersMutex_);
        profilers_.insert(profiler);
    }
    return profiler;
}

bool PGOProfilerManager::IsEnable() const
{
    return !disablePGO_ && isInitialized_;
}

bool PGOProfilerManager::InitializeData()
{
    LOG_PGO(INFO) << "check pgo profiler data";
    if (isInitialized_) {
        LOG_PGO(INFO) << "pgo profiler data is already initialized";
        return true;
    }
    if (!pgoInfo_ || !state_) {
        LOG_PGO(ERROR) << "pgo profiler data is not initialized properly";
        return false;
    }
    if (!ResetOutPath(ApNameUtils::DEFAULT_AP_NAME)) {
        LOG_PGO(ERROR) << "failed to reset ap file out path, output directory: " << outDir_;
        return false;
    }
    isInitialized_ = true;
    if (!enableSignalSaving_) {
        RegisterSavingSignal();
    }
    return true;
}

void PGOProfilerManager::Destroy(std::shared_ptr<PGOProfiler>& profiler)
{
    LOG_PGO(INFO) << "attempting to destroy pgo profiler: " << profiler;
    if (profiler != nullptr) {
        profiler->WaitDumpIfStart();
        profiler->HandlePGOPreDump();
        {
            LockHolder lock(profilersMutex_);
            profilers_.erase(profiler);
        }
        profiler.reset();
        LOG_PGO(INFO) << "pgo profiler destroied";
    }
}

void PGOProfilerManager::Reset(const std::shared_ptr<PGOProfiler>& profiler, bool isEnable)
{
    if (isEnable) {
        isEnable = InitializeData();
    }
    if (profiler) {
        profiler->Reset(isEnable);
    }
}

void PGOProfilerManager::SamplePandaFileInfo(uint32_t checksum, const CString& abcName)
{
    if (pgoInfo_) {
        pgoInfo_->SamplePandaFileInfoSafe(checksum, abcName);
    }
}

void PGOProfilerManager::SetModuleName(const std::string& moduleName)
{
    PostResetOutPathTask(moduleName);
}

bool PGOProfilerManager::GetPandaFileId(const CString& abcName, ApEntityId& entryId) const
{
    if (pgoInfo_) {
        return pgoInfo_->GetPandaFileIdSafe(abcName, entryId);
    }
    return false;
}

bool PGOProfilerManager::GetPandaFileDesc(ApEntityId abcId, CString& desc) const
{
    if (pgoInfo_) {
        return pgoInfo_->GetPandaFileDescSafe(abcId, desc);
    }
    return false;
}

void PGOProfilerManager::AsyncSave()
{
    PGOProfilerEncoder::PostSaveTask(outPath_, apGenMode_, pgoInfo_);
}

bool PGOProfilerManager::IsDisableAot() const
{
    return disableAot_;
}

void PGOProfilerManager::SetDisableAot(bool state)
{
    disableAot_ = state;
}

void PGOProfilerManager::SetDisablePGO(bool state)
{
    disablePGO_ = state;
}

void PGOProfilerManager::ForceDump()
{
    LockHolder lock(profilersMutex_);
    for (const auto& profiler: profilers_) {
        profiler->ForceDump();
    }
    state_->SetState(State::SAVE);
    GetInstance()->AsyncSave();
}

bool PGOProfilerManager::TextToBinary(const std::string& inPath,
                                      const std::string& outPath,
                                      uint32_t hotnessThreshold,
                                      ApGenMode mode)
{
    auto info = std::make_shared<PGOInfo>(hotnessThreshold);
    PGOProfilerEncoder encoder(outPath, mode);
    PGOProfilerDecoder decoder(inPath, hotnessThreshold);
    bool ret = decoder.LoadAPTextFile(inPath);
    if (ret) {
        info->MergeSafe(decoder.GetPandaFileInfos());
        info->MergeSafe(decoder.GetRecordDetailInfos());
        ret = encoder.Save(info);
    }
    info->Clear();
    return ret;
}

bool PGOProfilerManager::BinaryToText(const std::string& inPath,
                                      const std::string& outPath,
                                      uint32_t hotnessThreshold)
{
    PGOProfilerDecoder decoder(inPath, hotnessThreshold);
    if (!decoder.LoadFull()) {
        return false;
    }
    bool ret = decoder.SaveAPTextFile(outPath);
    decoder.Clear();
    return ret;
}

void PGOProfilerManager::SetIsApFileCompatible(bool isCompatible)
{
    isApFileCompatible_ = isCompatible;
}

bool PGOProfilerManager::GetIsApFileCompatible() const
{
    return isApFileCompatible_;
}

size_t PGOProfilerManager::GetMaxAotMethodSize() const
{
    return maxAotMethodSize_;
}

void PGOProfilerManager::SetMaxAotMethodSize(uint32_t value)
{
    maxAotMethodSize_ = value;
}

bool PGOProfilerManager::IsBigMethod(uint32_t methodSize) const
{
    return maxAotMethodSize_ != 0 && methodSize > maxAotMethodSize_;
}

std::shared_ptr<PGOState> PGOProfilerManager::GetPGOState() const
{
    return state_;
}

std::shared_ptr<PGOInfo> PGOProfilerManager::GetPGOInfo() const
{
    return pgoInfo_;
}

bool PGOProfilerManager::ResetOutPathByModuleName(const std::string& moduleName)
{
    LockHolder lock(resetOutPathMutex_);
    // only first assign takes effect
    if (!moduleName_.empty() || moduleName.empty()) {
        return false;
    }
    moduleName_ = moduleName;
    return ResetOutPath(ApNameUtils::GetRuntimeApName(moduleName_));
}

bool PGOProfilerManager::ResetOutPath(const std::string& fileName)
{
    return ResetOutPath(outDir_, outPath_, fileName);
}

bool PGOProfilerManager::ResetOutPath(const std::string& path, std::string& realPath, const std::string& fileName)
{
    if (!RealPath(path, realPath, false)) {
        LOG_PGO(ERROR) << "get real path failed, outDir: " << path;
        return false;
    }

    auto suffixLength = ApNameUtils::AP_SUFFIX.length();
    if (realPath.compare(realPath.length() - suffixLength, suffixLength, ApNameUtils::AP_SUFFIX)) {
        realPath += "/" + fileName;
    }

    SetSecurityLabel(realPath);

    LOG_PGO(INFO) << "will save profiler to file " << realPath;
    return true;
}

void PGOProfilerManager::RequestAot()
{
    if (bundleName_.empty() || moduleName_.empty()) {
        return;
    }

    LOG_ECMA(INFO) << "Request local aot, bundle: " << bundleName_ << ", module: " << moduleName_;
    if (!RequestAot(bundleName_, moduleName_, RequestAotMode::RE_COMPILE_ON_IDLE)) {
        LOG_ECMA(ERROR) << "Request aot failed, bundle: " << bundleName_ << ", module: " << moduleName_;
    }
}

void PGOProfilerManager::PostResetOutPathTask(const std::string& moduleName)
{
    if (moduleName.empty()) {
        LOG_PGO(ERROR) << "[" << __func__ << "] module name is empty.";
        return;
    }
    // only post moduleName once
    bool expected = false;
    bool desired = true;
    if (!hasPostModuleName_.compare_exchange_strong(expected, desired)) {
        return;
    }
    Taskpool::GetCurrentTaskpool()->PostTask(std::make_unique<ResetOutPathTask>(moduleName, GLOBAL_TASK_ID));
}

bool PGOProfilerManager::IsInitialized() const
{
    return isInitialized_;
}

void PGOProfilerManager::SetApGenMode(ApGenMode mode)
{
    apGenMode_ = mode;
}

ApGenMode PGOProfilerManager::GetApGenMode() const
{
    return apGenMode_;
}

Mutex& PGOProfilerManager::GetPGOInfoMutex()
{
    static Mutex pgoInfoMutex;
    return pgoInfoMutex;
}

ConcurrentGuardValue& PGOProfilerManager::GetConcurrentGuardValue()
{
    return v_;
}
} // namespace panda::ecmascript::pgo
