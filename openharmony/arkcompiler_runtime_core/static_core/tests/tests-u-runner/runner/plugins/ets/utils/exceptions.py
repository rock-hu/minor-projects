#!/usr/bin/env python3
# -*- coding: utf-8 -*-

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
#

class InvalidFileFormatException(Exception):
    def __init__(self, filepath: str, message: str) -> None:
        super().__init__(f"{filepath}: {message}")
        self.message = f"{filepath}: {message}"


class InvalidFileStructureException(Exception):
    def __init__(self, filepath: str, message: str) -> None:
        msg = f"{filepath}: {message}"
        super().__init__(msg)
        self.message = msg


class UnknownTemplateException(Exception):
    def __init__(self, filepath: str, exception: Exception) -> None:
        super().__init__(f"{filepath}: {str(exception)}")
        self.filepath = filepath
        self.exception = exception


class InvalidMetaException(Exception):
    def __init__(self, msg: str) -> None:
        super().__init__()
        self.message = msg
