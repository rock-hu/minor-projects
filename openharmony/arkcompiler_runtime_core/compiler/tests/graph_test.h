/**
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef GRAPH_TEST_H
#define GRAPH_TEST_H

#include "bytecode_optimizer/ir_interface.h"
#include "bytecode_optimizer/runtime_adapter.h"
#include "libpandafile/class_data_accessor.h"
#include "libpandafile/class_data_accessor-inl.h"
#include "libpandafile/code_data_accessor-inl.h"
#include "libpandafile/file.h"
#include "libpandafile/method_data_accessor.h"
#include "libpandabase/mem/arena_allocator.h"
#include "libpandabase/mem/pool_manager.h"
#include "optimizer/ir/graph.h"
#include "optimizer/ir/runtime_interface.h"
#include "optimizer/ir_builder/ir_builder.h"

namespace panda::compiler {
class GraphTest {
public:
    GraphTest()
    {
        PoolManager::Initialize(PoolType::MALLOC);
    }

    ~GraphTest()
    {
        PoolManager::Finalize();
    }

    static bool HasTryCatch(const panda_file::File &panda_file, panda_file::File::EntityId code_id)
    {
        panda_file::CodeDataAccessor cda(panda_file, code_id);
        return cda.GetTriesSize() != 0;
    }

    template <class Callback>
    void TestBuildGraphFromFile(const std::string &pfile_name, const Callback &cb, bool skip_try_catch = false)
    {
        auto pfile = panda_file::OpenPandaFile(pfile_name);
        for (uint32_t id : pfile->GetClasses()) {
            panda_file::File::EntityId record_id {id};
            if (pfile->IsExternal(record_id)) {
                continue;
            }

            panda_file::ClassDataAccessor cda {*pfile, record_id};
            cda.EnumerateMethods([&pfile, &cb, &skip_try_catch](panda_file::MethodDataAccessor &mda) {
                if (mda.IsExternal()) {
                    return;
                }

                // `skip_try_catch` is required for tests like draw_cfg_test. Currently try-catch optimizations
                // are disabled, and building graphs directly from abc files containing try-catch blocks may fail.
                if (skip_try_catch && HasTryCatch(*pfile, mda.GetCodeId().value())) {
                    return;
                }
                
                ArenaAllocator allocator {SpaceType::SPACE_TYPE_COMPILER};
                ArenaAllocator local_allocator {SpaceType::SPACE_TYPE_COMPILER, nullptr, true};

                auto method_ptr = reinterpret_cast<compiler::RuntimeInterface::MethodPtr>(
                    mda.GetMethodId().GetOffset());
                panda::BytecodeOptimizerRuntimeAdapter adapter(mda.GetPandaFile());
                auto *graph = allocator.New<Graph>(&allocator, &local_allocator, Arch::NONE, method_ptr, &adapter,
                                                    false, nullptr, true, true);
                graph->RunPass<panda::compiler::IrBuilder>();

                auto method_name = std::string(utf::Mutf8AsCString(pfile->GetStringData(mda.GetNameId()).data));
                auto pos = method_name.find_last_of("#");
                if (pos != std::string::npos) {
                    method_name = method_name.substr(pos + 1);
                }
                cb(graph, method_name);
            });
        }
    }
};

} // namespace panda::compiler

#endif  // GRAPH_TEST_H