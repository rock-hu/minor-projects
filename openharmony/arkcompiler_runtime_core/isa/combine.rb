#!/usr/bin/env ruby
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

# Huawei Technologies Co.,Ltd.

require 'optparse'
require 'ostruct'
require 'yaml'

def data_instructions(data)
  data['groups'].flat_map { |g| g['instructions'] }
end

options = OpenStruct.new
optparser = OptionParser.new do |opts|
  opts.banner = 'Usage: combine.rb [options]'

  opts.on('-d', '--data FILE1,FILE2,...', Array, 'List of source data files in YAML format')
  opts.on('-o', '--output FILE', 'Output file (default is stdout)')

  opts.on('-h', '--help', 'Prints this help') do
    puts opts
    exit
  end
end
optparser.parse!(into: options)

exit unless options.data
exit if options.data.empty?

data = YAML.load_file(File.expand_path(options.data.first))
options.data.drop(1).each do |plugin_path|
  plugin_data = YAML.load_file(File.expand_path(plugin_path))
  # check that all instructions are prefixed:
  instructions = data_instructions(plugin_data)
  raise 'Plugged in instructions must be prefixed' unless instructions.reject { |i| i['prefix'] }.empty?

  plugin_data.each_key do |attr|
    raise "Uknown data property: #{attr}" unless data.key?(attr)

    data[attr] += plugin_data[attr]
  end
end

output = options.output ? File.open(File.expand_path(options.output), 'w') : $stdout
output.write(data.to_yaml)
output.close
