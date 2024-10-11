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

#ifndef LIBPANDABASE_UTILS_TSAN_INTERFACE_H
#define LIBPANDABASE_UTILS_TSAN_INTERFACE_H

#if defined(__has_feature)
// for clang
#if __has_feature(thread_sanitizer)
#define PANDA_TSAN_ON
#endif
#else
#if defined(__SANITIZE_THREAD__)
// for gnu compiler
#define PANDA_TSAN_ON
#endif
#endif

#endif  // LIBPANDABASE_UTILS_TSAN_INTERFACE_H
