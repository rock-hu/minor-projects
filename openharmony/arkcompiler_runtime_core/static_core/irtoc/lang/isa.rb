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

require 'yaml'
require 'json'

module Gen; end

class Module
  def cached(method_name)
    definer = instance_methods.include?(method_name) ? :define_method : :define_singleton_method
    noncached_method = instance_method(method_name)
    send(definer, method_name) do
      unless instance_variable_defined? "@#{method_name}"
        instance_variable_set("@#{method_name}", noncached_method.bind(self).call)
      end
      instance_variable_get("@#{method_name}").freeze
    end
  end
end

class ISA
  def self.setup(isa_filename, isapi_filename)
    isa = YAML.load_file(File.expand_path(isa_filename))
    isa = JSON.parse(isa.to_json, object_class: OpenStruct).freeze
    require isapi_filename
    Gen.on_require(isa)
  end
end
