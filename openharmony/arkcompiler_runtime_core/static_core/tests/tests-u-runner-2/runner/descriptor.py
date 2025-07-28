#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#

import re
from typing import Any


class Descriptor:
    def __init__(self, input_file: str) -> None:
        self.input_file = input_file
        self.header = re.compile(
            r"/\*---(?P<header>.+)---\*/", re.DOTALL)
        self.includes = re.compile(r"includes:\s+\[(?P<includes>.+)]")
        spaces_pattern = r"(\s+-[^-].+)+"
        self.includes2 = re.compile(f"includes:(?P<includes>{spaces_pattern})")
        self.flags = re.compile(r"flags:\s+\[(?P<flags>.+)]")
        self.negative = re.compile(
            r"negative:.*phase:\s+(?P<phase>\w+).*type:\s+(?P<type>\w+)",
            re.DOTALL)
        self.spec = re.compile(r"spec:\s+(?P<spec>.+)")
        self.issues = re.compile(r"issues:\s+(?P<issues>.+)")

        self.content = self.get_content()

    def get_content(self) -> str:
        with open(self.input_file, encoding="utf-8") as file_pointer:
            input_str = file_pointer.read()
        return input_str

    def get_header(self) -> str:
        header_comment = self.header.search(self.content)
        return header_comment.group(0) if header_comment else ""

    def parse_descriptor(self) -> dict[str, Any]:   # type: ignore[explicit-any]
        header = self.get_header()
        result: dict[str, Any] = {}     # type: ignore[explicit-any]

        if len(header) == 0:
            return result

        includes = []
        match = self.includes.search(header)
        includes += [incl.strip() for incl in match.group("includes").split(",")] if match else []

        match = self.includes2.search(header)
        includes += [incl.strip() for incl in match.group("includes").split("-")][1:] if match else []

        result["includes"] = includes

        match = self.flags.search(header)
        if match:
            result["flags"] = [flag.strip() for flag in match.group("flags").split(",")]

        match = self.negative.search(header)
        if match:
            result["negative_phase"] = match.group("phase")
            result["negative_type"] = match.group("type")

        match = self.spec.search(header)
        if match:
            result["spec"] = match.group("spec")

        match = self.issues.search(header)
        if match:
            result["issues"] = [issue.strip() for issue in match.group("issues").split(",")]

        return result

    def get_descriptor(self) -> dict[str, Any]:     # type: ignore[explicit-any]
        return self.parse_descriptor()
