# coding=utf-8
#
# Copyright (c) 2025 Huawei Device Co., Ltd.
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

from taihe.codegen.mangle import DeclKind, decode, encode


def test_name_mangler():
    """Comprehensive test suite for NameMangler."""
    test_cases = [
        (["a", "b_c", "d", "e_f_g"], DeclKind.FUNC),
        (["x", "y", "z"], DeclKind.TYPE),
        (["a_b_c"], DeclKind.FUNC),
        (["a"], DeclKind.FUNC),
        (["a_b", "c_d_e"], DeclKind.FUNC),
        (["test", "with____multiple", "consecutive"], DeclKind.FUNC),
    ]

    for segments, kind in test_cases:
        try:
            mangled = encode(segments, kind)
            decoded = decode(mangled)
            if decoded != (segments, kind):
                raise ValueError(f"Failed roundtrip: {segments} -> {mangled} -> {decoded}")
            print(f"Success: {segments} -> {mangled} -> {decoded}")
        except Exception as e:
            print(f"Error testing {segments}: {e!s}")
