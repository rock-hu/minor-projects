/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef COMMON_COMPONENTS_BASE_CONFIG_H
#define COMMON_COMPONENTS_BASE_CONFIG_H

namespace common {
    
#if defined (NEXT_OPTIMIZATION_MACRO)
#define ENABLE_NEXT_OPTIMIZATION 1
#else
#define ENABLE_NEXT_OPTIMIZATION 0
#endif

#if ENABLE_NEXT_OPTIMIZATION
    #define NEXT_OPTIMIZATION_BOOL true
#else
    #define NEXT_OPTIMIZATION_BOOL false
#endif

}  // namespace common

#endif  // COMMON_COMPONENTS_BASE_CONFIG_H