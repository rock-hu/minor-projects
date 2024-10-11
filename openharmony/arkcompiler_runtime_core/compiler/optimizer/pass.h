/**
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

#ifndef COMPILER_OPTIMIZER_PASS_H
#define COMPILER_OPTIMIZER_PASS_H

#include <string>
#include "utils/bit_field.h"

namespace panda::compiler {
class Graph;

class Pass {
public:
    explicit Pass(Graph *graph) : graph_(graph) {}
    virtual ~Pass() = default;

    /**
     * Run pass execution.
     * @return true if succeeded.
     */
    virtual bool RunImpl() = 0;

    virtual const char *GetPassName() const = 0;

    virtual bool AbortIfFailed() const
    {
        return false;
    }

    virtual bool ShouldDump() const = 0;

    bool Run();

    Graph *GetGraph() const
    {
        return graph_;
    }

    bool IsAnalysis() const
    {
        return ReadField<IsAnalysisField>();
    }

    bool IsValid() const
    {
        return ReadField<IsValidField>();
    }

    void SetValid(bool value)
    {
        WriteField<IsValidField>(value);
    }

    template <typename T>
    typename T::ValueType ReadField() const
    {
        return T::Decode(bit_fields_);
    }

    template <typename T>
    void WriteField(typename T::ValueType v)
    {
        T::Set(v, &bit_fields_);
    }

    NO_MOVE_SEMANTIC(Pass);
    NO_COPY_SEMANTIC(Pass);

protected:
    using IsAnalysisField = BitField<bool, 0>;
    using IsValidField = IsAnalysisField::NextFlag;
    using LastField = IsValidField;

private:
    Graph *graph_ {nullptr};
    uint32_t bit_fields_ {0};
};

class Optimization : public Pass {
public:
    using Pass::Pass;
    virtual bool IsEnable() const
    {
        return true;
    }

    bool ShouldDump() const override
    {
        return true;
    }
    virtual void InvalidateAnalyses() {}
};

class Analysis : public Pass {
public:
    explicit Analysis(Graph *graph) : Pass(graph)
    {
        WriteField<IsAnalysisField>(true);
    }
    NO_MOVE_SEMANTIC(Analysis);
    NO_COPY_SEMANTIC(Analysis);
    ~Analysis() override = default;

    /**
     * Is the IR should be dumped after the pass finished.
     */
    bool ShouldDump() const override
    {
        return false;
    }
};
}  // namespace panda::compiler

#endif  // COMPILER_OPTIMIZER_PASS_H
