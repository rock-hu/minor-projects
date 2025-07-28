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

import sys
from dataclasses import dataclass
from typing import TYPE_CHECKING

from taihe.driver.backend import Backend, BackendConfig
from taihe.utils.outputs import DEFAULT_INDENT

if TYPE_CHECKING:
    from taihe.driver.contexts import CompilerInstance


@dataclass
class PrettyPrintBackendConfig(BackendConfig):
    NAME = "pretty-print"

    show_resolved = True
    colorize = True

    def construct(self, instance: "CompilerInstance") -> Backend:
        from taihe.semantics.format import PrettyPrinter
        from taihe.utils.outputs import BaseWriter

        class PrettyPrintBackendImpl(Backend):
            def __init__(self, ci: "CompilerInstance", config: BackendConfig):
                super().__init__(ci)
                pass
                self._ci = ci
                self._config = config

            def generate(self):
                PrettyPrinter(
                    BaseWriter(
                        sys.stdout,
                        default_indent=DEFAULT_INDENT,
                        comment_prefix="// ",
                    ),
                    self._config.show_resolved,
                    self._config.colorize,
                ).handle_decl(self._ci.package_group)

        return PrettyPrintBackendImpl(instance, self)