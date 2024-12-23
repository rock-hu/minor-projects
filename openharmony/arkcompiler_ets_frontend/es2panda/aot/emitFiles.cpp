/*
 * Copyright (c) 2023 - 2024 Huawei Device Co., Ltd.
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

#include "emitFiles.h"

#include <assembly-emitter.h>
#include "utils/timers.h"

#include <es2panda.h>
#include <protobufSnapshotGenerator.h>
#include <util/commonUtil.h>
#include <util/helpers.h>

namespace panda::es2panda::aot {
void EmitFileQueue::ScheduleEmitCacheJobs(EmitMergedAbcJob *emitMergedAbcJob)
{
    std::unordered_map<std::string, util::AbcProgramsCache *> abcProgsCacheMap {};
    /**
     * The programKey is usually the fileName of the source file. If the program generated from the abc file,
     * the programKey format is the abc filePath + verticalLine + recordName.
     */
    for (const auto &[programKey, programCache]: progsInfo_) {
        // generate cache protoBins and set dependencies
        if (!programCache->needUpdateCache) {
            continue;
        }

        bool isAbcProgram = options_->CompilerOptions().enableAbcInput &&
                            programKey.find(util::CHAR_VERTICAL_LINE) != std::string::npos;
        if (isAbcProgram) {
            // An abc file will contain multiple programs, and a protobin file is needed to map multiple programs
            FillAbcProgramsMap(abcProgsCacheMap, programKey, programCache);
            continue;
        }
        auto outputCacheIter = options_->CompilerOptions().cacheFiles.find(programKey);
        if (outputCacheIter != options_->CompilerOptions().cacheFiles.end()) {
            auto emitProtoJob = new EmitCacheJob(outputCacheIter->second, programCache);
            emitProtoJob->DependsOn(emitMergedAbcJob);
            jobs_.push_back(emitProtoJob);
            jobsCount_++;
        }
    }
    for (const auto &info: abcProgsCacheMap) {
        // Cache programs from the same abc file into a protobin file
        auto outputCacheIter = options_->CompilerOptions().cacheFiles.find(info.first);
        auto emitAbcProtoJob = new EmitAbcCacheJob(outputCacheIter->second, info.second);
        emitAbcProtoJob->DependsOn(emitMergedAbcJob);
        jobs_.push_back(emitAbcProtoJob);
        jobsCount_++;
    }
}

void EmitFileQueue::FillAbcProgramsMap(std::unordered_map<std::string, util::AbcProgramsCache *> &abcprogsCacheMap,
                                       std::string progKey, panda::es2panda::util::ProgramCache *progCache)
{
    constexpr size_t ABC_FILE_PATH_POS = 0;
    std::vector<std::string> items = util::Split(progKey, util::CHAR_VERTICAL_LINE);
    std::string abcFilePath = items[ABC_FILE_PATH_POS];
    auto outputCacheIter = options_->CompilerOptions().cacheFiles.find(abcFilePath);
    if (outputCacheIter != options_->CompilerOptions().cacheFiles.end()) {
        auto iter = abcprogsCacheMap.find(abcFilePath);
        if (iter == abcprogsCacheMap.end()) {
            std::map<std::string, util::ProgramCache *> abcProgramsInfo {};
            abcProgramsInfo.insert({progKey, progCache});
            auto *cache = allocator_->New<panda::es2panda::util::AbcProgramsCache>(progCache->hashCode,
                                                                                   abcProgramsInfo);
            abcprogsCacheMap.insert({abcFilePath, cache});
        } else {
            iter->second->programsCache.insert({progKey, progCache});
        }
    }
}

void EmitFileQueue::Schedule()
{
    ASSERT(jobsCount_ == 0);
    std::unique_lock<std::mutex> lock(m_);
    auto targetApi = options_->CompilerOptions().targetApiVersion;
    auto targetSubApi = options_->CompilerOptions().targetApiSubVersion;

    if (mergeAbc_) {
        // generate merged abc
        auto emitMergedAbcJob = new EmitMergedAbcJob(options_->CompilerOutput(),
            options_->CompilerOptions().transformLib, progsInfo_, targetApi, targetSubApi);
        // Disable generating cached files when cross-program optimization is required, to prevent cached files from
        // not being invalidated when their dependencies are changed
        if (!options_->CompilerOptions().requireGlobalOptimization) {
            ScheduleEmitCacheJobs(emitMergedAbcJob);
        }
        //  One job should be placed after those jobs which depend on it to prevent blocking
        jobs_.push_back(emitMergedAbcJob);
        jobsCount_++;
    } else {
        for (const auto &info: progsInfo_) {
            try {
                // generate multi abcs
                auto outputFileName = options_->OutputFiles().empty() ? options_->CompilerOutput() :
                    options_->OutputFiles().at(info.first);
                auto emitSingleAbcJob = new EmitSingleAbcJob(outputFileName, &(info.second->program), statp_,
                                                             targetApi, targetSubApi);
                jobs_.push_back(emitSingleAbcJob);
                jobsCount_++;
            } catch (std::exception &error) {
                throw Error(ErrorType::GENERIC, error.what());
            }
        }
    }

    lock.unlock();
    jobsAvailable_.notify_all();
}

void EmitSingleAbcJob::Run()
{
    panda::Timer::timerStart(panda::EVENT_EMIT_SINGLE_PROGRAM, outputFileName_);
    if (!panda::pandasm::AsmEmitter::Emit(panda::os::file::File::GetExtendedFilePath(outputFileName_), *prog_, statp_,
        nullptr, true, nullptr, targetApiVersion_, targetApiSubVersion_)) {
        throw Error(ErrorType::GENERIC, "Failed to emit " + outputFileName_ + ", error: " +
            panda::pandasm::AsmEmitter::GetLastError());
    }
    for (auto *dependant : dependants_) {
        dependant->Signal();
    }
    panda::Timer::timerEnd(panda::EVENT_EMIT_SINGLE_PROGRAM, outputFileName_);
}

void EmitMergedAbcJob::Run()
{
    panda::Timer::timerStart(panda::EVENT_EMIT_MERGED_PROGRAM, "");
    std::vector<panda::pandasm::Program*> progs;
    progs.reserve(progsInfo_.size());
    for (const auto &info: progsInfo_) {
        progs.push_back(&(info.second->program));
    }

    bool success = panda::pandasm::AsmEmitter::EmitPrograms(
        panda::os::file::File::GetExtendedFilePath(outputFileName_), progs, true,
        targetApiVersion_, targetApiSubVersion_);

    for (auto *dependant : dependants_) {
        dependant->Signal();
    }
    panda::Timer::timerEnd(panda::EVENT_EMIT_MERGED_PROGRAM, "");

    if (!success) {
        throw Error(ErrorType::GENERIC, "Failed to emit " + outputFileName_ + ", error: " +
            panda::pandasm::AsmEmitter::GetLastError() +
            "\nIf you're using any cache file generated by older version of SDK, " +
            "please try cleaning the cache files and rebuild");
    }

    if (!transformLib_.empty()) {
        util::Helpers::AopTransform(outputFileName_, transformLib_);
    }
}

void EmitCacheJob::Run()
{
    std::unique_lock<std::mutex> lock(m_);
    cond_.wait(lock, [this] { return dependencies_ == 0; });
    panda::Timer::timerStart(panda::EVENT_EMIT_CACHE_FILE, outputProtoName_);
    panda::proto::ProtobufSnapshotGenerator::UpdateCacheFile(progCache_, outputProtoName_);
    panda::Timer::timerEnd(panda::EVENT_EMIT_CACHE_FILE, outputProtoName_);
}

void EmitAbcCacheJob::Run()
{
    std::unique_lock<std::mutex> lock(m_);
    cond_.wait(lock, [this] { return dependencies_ == 0; });
    panda::Timer::timerStart(panda::EVENT_UPDATE_ABC_PROG_CACHE, outputProtoName_);
    panda::proto::ProtobufSnapshotGenerator::UpdateAbcCacheFile(abcProgsCache_, outputProtoName_);
    panda::Timer::timerEnd(panda::EVENT_UPDATE_ABC_PROG_CACHE, outputProtoName_);
}

}  // namespace panda::es2panda::util
