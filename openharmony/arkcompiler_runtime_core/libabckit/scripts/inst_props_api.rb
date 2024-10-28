# Copyright (c) 2024 Huawei Device Co., Ltd.
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
require 'ostruct'

abckit_scripts = File.dirname(__FILE__)
abckit_root    = File.expand_path('../', abckit_scripts)
opcodes_path = File.expand_path('include/c/isa/isa_dynamic.h', abckit_root)

Opcodes = []
File.open(opcodes_path, 'r').read.split()
  .select{ |opcode|
    opcode.start_with?('ABCKIT_ISA_API_DYNAMIC_OPCODE_')
  }.each{ |opcode|
    res = OpenStruct.new
    res.abckit_opcode = opcode.chop!
    res.bc_opcode = opcode.delete_prefix('ABCKIT_ISA_API_DYNAMIC_OPCODE_').split('_').map!{|str| str.downcase }.join('.')
    Opcodes.append(res)
  }
