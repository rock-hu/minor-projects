#!/usr/bin/env ruby
# Copyright (c) 2021-2024 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

module GeneratorTags
  class Calculator
    AVAILABLE_REGISTERS = %w[v5 v6 v7 v8]
    PRIMITIVE_TAG = 0
    OBJECT_TAG = 1
    def refresh
      @free_registers = AVAILABLE_REGISTERS.clone
    end

    def free_register
      free_reg = @free_registers[0]
      @free_registers.delete_at(0)
      free_reg
    end

    def calculate_expected_tag(type)
      val = PRIMITIVE_TAG
      val = OBJECT_TAG if type.include?('ref') || type.include?('[]')
      val
    end
  end
end
