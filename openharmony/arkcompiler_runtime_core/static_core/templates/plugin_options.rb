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

require 'ostruct'
require 'delegate'

module Common
  module_function

  def plugins
    @plugins
  end

  def each_plugin_option(option)
    @plugins.each do |plugin, plugin_opts|
      next unless plugin_opts[option]
      yield plugin_opts[option], plugin, plugin_opts
    end
  end

  def each_plugin_suboption(option, suboption)
    @plugins.each do |plugin, plugin_opts|
      next unless plugin_opts[option]
      next unless plugin_opts[option][suboption]
      yield plugin_opts[option][suboption], plugin, plugin_opts
    end
  end

  def include_plugin_files(option, suboption)
    @plugins
      .select { |plugin, plugin_opts| plugin_opts[option] && plugin_opts[option][suboption] }
      .map { |plugin, plugin_opts| "#include \"#{plugin_opts[option][suboption]}\"" }
      .join "\n"
  end

  def assign_data_level(cur_hash, key, cur_data)
    if !cur_data && (cur_data.class == OpenStruct || cur_data.class == Array)
      return
    elsif !cur_data
      #boolean ?
      cur_data = "false"
    end

    if cur_data.class == OpenStruct
      wrap_hash_level(cur_hash, key, cur_data)
    elsif cur_data.class == Array
      wrap_array_level(cur_hash, key, cur_data)
    else
      if !cur_hash[key]
        cur_hash[key] = cur_data.to_s
      elsif cur_hash[key].class == Array
        cur_hash[key].append(cur_data.to_s)
      else
        cur_hash[key] = [cur_hash[key]].append(cur_data.to_s)
      end
    end
  end

  def wrap_array_level(cur_hash, key, cur_data)
    if !cur_data || cur_data.class != Array
      return
    end

    cur_data.each do |val|
      assign_data_level(cur_hash, key, val)
    end
  end

  def wrap_hash_level(cur_hash, key, cur_data)
    if !cur_data || cur_data.class != OpenStruct
        return
    end

    if !cur_hash[key]
      cur_hash[key] = Hash.new()
    end

    new_h = cur_data.to_h

    new_h.each do |sub_key, val|
      assign_data_level(cur_hash[key.to_s], sub_key.to_s, val)
    end
  end

  def wrap_data(data)
    @data = data
    @plugins = Hash.new()
    if !data || !data.plugins
      return
    end

    data.plugins.each do |plugin|
      h_plugin = plugin.to_h
      h_plugin_lang = h_plugin.keys.first.to_s
      lang_enum = "ark::SourceLanguage::" + h_plugin_lang
      @plugins[h_plugin_lang] = Hash.new()
      @plugins[h_plugin_lang]["lang_enum"] = lang_enum
      assign_data_level(@plugins, h_plugin_lang, h_plugin.values.first)
    end
  end
end

def Gen.on_require(data)
  Common.wrap_data(data)
end
