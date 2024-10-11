# Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

require 'delegate'

class Intrinsic < SimpleDelegator

  TYPES = {
    "u1"  => "BOOL",
    "i8" => "INT8",
    "u8" => "UINT8",
    "i16" => "INT16",
    "u16" => "UINT16",
    "i32" => "INT32",
    "u32" => "UINT32",
    "i64" => "INT64",
    "u64" => "UINT64",
    "f32"  => "FLOAT32",
    "f64"  => "FLOAT64",
    "void" => "VOID"
  }

  def enum_name
    res = name.gsub(/([A-Z]+)([A-Z][a-z])/,'\1_\2')
    res.gsub(/([a-z\d])([A-Z])/,'\1_\2').upcase
  end

  def entrypoint_name
    'INTRINSIC_' + enum_name
  end

  def return_type
    TYPES[signature.ret] || "REFERENCE"
  end

  def arguments
    (signature.args.length() < impl_signature.args.length() ? ["REFERENCE"] : []) +
        signature.args.map {|arg| TYPES[arg] || "REFERENCE" }
  end

  def has_impl?
    respond_to?(:impl)
  end

  def is_irtoc?
    class_name == 'Irtoc'
  end

  def is_dynamic?
    signature.ret == "any" || signature.args.include?("any")
  end
end

module Compiler
  module_function

  def intrinsics
    @exclude_list = [
    ]

    @data.intrinsics.select { |i| !@exclude_list.include?(i.name) }.map do |intrinsic|
      Intrinsic.new(intrinsic)
    end
  end

  def wrap_data(data)
    @data = data
    @ext_intrinsic_spaces = Compiler::intrinsics.collect {|intrinsic| intrinsic.space}.select {|space| space != 'core'}.uniq
  end

  def ext_intrinsic_spaces
    @ext_intrinsic_spaces
  end
end

def Gen.on_require(data)
  Compiler.wrap_data(data)
end
