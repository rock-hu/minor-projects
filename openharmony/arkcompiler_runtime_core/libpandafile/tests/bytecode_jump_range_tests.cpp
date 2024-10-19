/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <sstream>
#include <type_traits>

#include "bytecode_instruction-inl.h"

namespace panda::test {
    
TEST(BytecodeInstruction, IsJumpInstruction)
{
    {
        // jmp -22
        const uint8_t bytecode[] = {0x4d, 0xea};
        BytecodeInstruction inst(bytecode);
        EXPECT_EQ(inst.IsJumpInstruction(), true);
    }
    {
        // a non-jump instruction
        const uint8_t bytecode[] = {0x08, 0x0b, 0x03, 0x06};
        BytecodeInstruction inst(bytecode);
        EXPECT_EQ(inst.IsJumpInstruction(), false);
    }
}

TEST(BytecodeInstruction, IsRangeInstruction)
{
    {
        // newobjrange 0xb, 0x3, v6
        const uint8_t bytecode[] = {0x08, 0x0b, 0x03, 0x06};
        BytecodeInstruction inst(bytecode);
        EXPECT_EQ(inst.IsRangeInstruction(), true);
    }
    {
        // a non-range instruction
        const uint8_t bytecode[] = {0x44, 0x58};
        BytecodeInstruction inst(bytecode);
        EXPECT_EQ(inst.IsRangeInstruction(), false);
    }
}

TEST(BytecodeInstruction, GetRangeInsLastRegIdx) {
    {
        // a non-range instruction
        const uint8_t bytecode[] = {0x3f, 0x0a, 0x00, 0x00};
        BytecodeInstruction inst(bytecode);
        EXPECT_EQ(inst.GetRangeInsLastRegIdx(), std::nullopt);
    }
    {
        // newobjrange 0xb, 0x3, v6
        const uint8_t bytecode[] = {0x08, 0x0b, 0x03, 0x06};
        BytecodeInstruction inst(bytecode);
        EXPECT_EQ(inst.GetRangeInsLastRegIdx().value(), 8);
    }
}
}