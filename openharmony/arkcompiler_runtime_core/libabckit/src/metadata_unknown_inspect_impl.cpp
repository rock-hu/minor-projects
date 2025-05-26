/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and limitations under the
 * License.
 */

#include <cassert>
#include <cstdint>
#include "libabckit/include/c/abckit.h"

#include "libabckit/src/macros.h"
#include "libabckit/src/metadata_unknown_inspect_impl.h"
#include "libabckit/src/metadata_inspect_impl.h"
#include "scoped_timer.h"

namespace libabckit {

// ========================================
// File
// ========================================

// ========================================
// Module
// ========================================

bool UnknownModuleEnumerateStub(AbckitCoreModule *m)
{
    LIBABCKIT_CLEAR_LAST_ERROR;
    LIBABCKIT_IMPLEMENTED;
    LIBABCKIT_TIME_EXEC;

    LIBABCKIT_BAD_ARGUMENT(m, false);

    if (m->target != ABCKIT_TARGET_UNKNOWN) {
        libabckit::statuses::SetLastError(ABCKIT_STATUS_WRONG_TARGET);
        return false;
    }

    return true;
}

// ========================================
// Class
// ========================================

// ========================================
// AnnotationInterface
// ========================================

// ========================================
// Function
// ========================================

// ========================================
// Annotation
// ========================================

// ========================================
// Type
// ========================================

// ========================================
// Value
// ========================================

// ========================================
// String
// ========================================

// ========================================
// LiteralArray
// ========================================

}  // namespace libabckit
