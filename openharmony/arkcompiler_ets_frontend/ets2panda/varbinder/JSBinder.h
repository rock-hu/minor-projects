/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef ES2PANDA_VARBINDER_JS_VARBINDER_H
#define ES2PANDA_VARBINDER_JS_VARBINDER_H

#include "varbinder/varbinder.h"

namespace ark::es2panda::varbinder {
class JSBinder : public VarBinder {
public:
    explicit JSBinder(ArenaAllocator *allocator) : VarBinder(allocator) {}

    NO_COPY_SEMANTIC(JSBinder);
    NO_MOVE_SEMANTIC(JSBinder);
    ~JSBinder() override = default;

private:
};
}  // namespace ark::es2panda::varbinder

#endif
