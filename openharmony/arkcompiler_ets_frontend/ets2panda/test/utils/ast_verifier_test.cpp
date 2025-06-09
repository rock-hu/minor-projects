/**
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "ast_verifier_test.h"

namespace test::utils {

AstVerifierTest::AstVerifierTest()
{
    impl_ = es2panda_GetImpl(ES2PANDA_LIB_VERSION);
    auto es2pandaPath = test::utils::PandaExecutablePathGetter::Get()[0];
    std::array argv = {es2pandaPath};
    cfg_ = impl_->CreateConfig(argv.size(), argv.data());
    allocator_ = new ark::ArenaAllocator(ark::SpaceType::SPACE_TYPE_COMPILER);
}

AstVerifierTest::~AstVerifierTest()
{
    ASSERT(ctx_ == nullptr);
    delete allocator_;
    impl_->DestroyConfig(cfg_);
}

}  // namespace test::utils