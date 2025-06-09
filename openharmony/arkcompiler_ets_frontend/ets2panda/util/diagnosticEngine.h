/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_UTIL_DIAGNOSTIC_ENGINE_H
#define ES2PANDA_UTIL_DIAGNOSTIC_ENGINE_H

#include <memory>
#include <utility>
#include "es2panda.h"
#include "util/es2pandaMacros.h"
#include "generated/diagnostic.h"
#include "util/diagnostic.h"
#include "lexer/token/sourceLocation.h"

namespace ark::es2panda::util {

class DiagnosticPrinter {
public:
    DiagnosticPrinter() = default;
    NO_COPY_SEMANTIC(DiagnosticPrinter);
    NO_MOVE_SEMANTIC(DiagnosticPrinter);
    virtual ~DiagnosticPrinter() = default;

    virtual void Print(const DiagnosticBase &diagnostic) const = 0;
};

class CLIDiagnosticPrinter : public DiagnosticPrinter {
public:
    CLIDiagnosticPrinter() = default;
    NO_COPY_SEMANTIC(CLIDiagnosticPrinter);
    NO_MOVE_SEMANTIC(CLIDiagnosticPrinter);
    ~CLIDiagnosticPrinter() override = default;

    void Print(const DiagnosticBase &diagnostic) const override;
};

using DiagnosticStorage = std::vector<std::unique_ptr<DiagnosticBase>>;

class DiagnosticEngine {
public:
    explicit DiagnosticEngine() : printer_(std::make_unique<CLIDiagnosticPrinter>())
    {
        g_diagnosticEngine = this;
    }
    NO_COPY_SEMANTIC(DiagnosticEngine);
    NO_MOVE_SEMANTIC(DiagnosticEngine);
    ~DiagnosticEngine()
    {
        FlushDiagnostic();
        g_diagnosticEngine = nullptr;
    }

    // NOTE(schernykh): should be removed
    const DiagnosticBase &GetAnyError() const;

    [[nodiscard]] bool IsAnyError() const noexcept;

    template <typename... T>
    void LogDiagnostic(T &&...args)
    {
        LogDiagnostic<Diagnostic>(std::forward<T>(args)...);
    }

    // NOTE(schernykh): should be removed
    void Log([[maybe_unused]] const ThrowableDiagnostic &error)
    {
        printer_->Print(error);
    };

    template <typename... T>
    void LogSyntaxError(T &&...args)
    {
        LogThrowableDiagnostic(DiagnosticType::SYNTAX, std::forward<T>(args)...);
    }
    template <typename... T>
    void LogSemanticError(T &&...args)
    {
        LogThrowableDiagnostic(DiagnosticType::SEMANTIC, std::forward<T>(args)...);
    }
    template <typename... T>
    void LogFatalError(T &&...args)
    {
        LogThrowableDiagnostic(DiagnosticType::FATAL, std::forward<T>(args)...);
    }
    template <typename... T>
    void LogWarning(T &&...args)
    {
        LogThrowableDiagnostic(DiagnosticType::WARNING, std::forward<T>(args)...);
    }

    // NOTE(schernykh): should not be able from ETS
    template <typename... T>
    [[noreturn]] void ThrowSyntaxError(T &&...args)
    {
        ThrowDiagnostic(DiagnosticType::SYNTAX, std::forward<T>(args)...);
    }
    template <typename... T>
    [[noreturn]] void ThrowSemanticError(T &&...args)
    {
        ThrowDiagnostic(DiagnosticType::SEMANTIC, std::forward<T>(args)...);
    }
    template <typename... T>
    [[noreturn]] void ThrowFatalError(T &&...args)
    {
        ThrowDiagnostic(DiagnosticType::FATAL, std::forward<T>(args)...);
    }

    void FlushDiagnostic();
    void SetWError(bool wError)
    {
        wError_ = wError;
    }

    const DiagnosticStorage &GetDiagnosticStorage(DiagnosticType type);

    static void InitializeSignalHandlers();

private:
    template <typename DIAGNOSTIC, typename... T>
    void LogDiagnostic(T &&...args)
    {
        auto diag = std::make_unique<DIAGNOSTIC>(std::forward<T>(args)...);
        auto type = diag->Type();
        diagnostics_[type].push_back(std::move(diag));
    }

    template <typename... T>
    void LogThrowableDiagnostic(T &&...args)
    {
        LogDiagnostic<ThrowableDiagnostic>(std::forward<T>(args)...);
    }

    template <typename... T>
    [[noreturn]] void ThrowDiagnostic(T &&...args) const
    {
        Throw(ThrowableDiagnostic {std::forward<T>(args)...});
    }
    [[noreturn]] void Throw(ThrowableDiagnostic diag) const;

    bool IsError(DiagnosticType type) const;
    DiagnosticStorage GetAllDiagnostic();
    void WriteLog(const DiagnosticBase &error);

private:
    std::array<DiagnosticStorage, static_cast<size_t>(DiagnosticType::COUNT)> diagnostics_;
    std::unique_ptr<const DiagnosticPrinter> printer_;
    bool wError_ {false};
};

}  // namespace ark::es2panda::util

#endif  // ES2PANDA_UTIL_DIAGNOSTIC_ENGINE_H
