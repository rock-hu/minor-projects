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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_ANI_CHECKERS_H
#define PANDA_PLUGINS_ETS_RUNTIME_ANI_CHECKERS_H
// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// CC-OFFNXT(G.PRE.02) should be with define
#define ANI_DEBUG_TRACE(env)

// CC-OFFNXT(G.PRE.02) should be with define
#define ANI_CHECK_RETURN_IF_EQ(a, b, ret) \
    do {                                  \
        if (UNLIKELY((a) == (b))) {       \
            return ret;                   \
        }                                 \
    } while (false)

// CC-OFFNXT(G.PRE.02) should be with define
#define ANI_CHECK_RETURN_IF_NE(a, b, ret) \
    do {                                  \
        if (UNLIKELY((a) != (b))) {       \
            return ret;                   \
        }                                 \
    } while (false)

// CC-OFFNXT(G.PRE.02) should be with define
#define ANI_CHECK_RETURN_IF_GT(a, b, ret) \
    do {                                  \
        if (UNLIKELY((a) > (b))) {        \
            return ret;                   \
        }                                 \
    } while (false)

// CC-OFFNXT(G.PRE.02) should be with define
#define ANI_CHECK_RETURN_IF_GE(a, b, ret) \
    do {                                  \
        if (UNLIKELY((a) >= (b))) {       \
            return ret;                   \
        }                                 \
    } while (false)

// CC-OFFNXT(G.PRE.02) should be with define
#define ANI_CHECK_RETURN_IF_LT(a, b, ret) \
    do {                                  \
        if (UNLIKELY((a) < (b))) {        \
            return ret;                   \
        }                                 \
    } while (false)

// CC-OFFNXT(G.PRE.02) should be with define
#define ANI_CHECK_RETURN_IF_LE(a, b, ret) \
    do {                                  \
        if (UNLIKELY((a) <= (b))) {       \
            return ret;                   \
        }                                 \
    } while (false)

// NOLINTEND(cppcoreguidelines-macro-usage)
#endif  // PANDA_PLUGINS_ETS_RUNTIME_ANI_CHECKERS_H
