#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

from functools import cached_property
from pathlib import Path
from typing import Optional, List, Any, Dict, cast

from runner import utils
from runner.logger import Log
from runner.options.macros import Macros, ParameterNotFound
from runner.options.options import IOptions

_LOGGER = Log.get_logger(__file__)


class CollectionsOptions(IOptions):
    __TEST_ROOT = "test-root"
    __LIST_ROOT = "list-root"
    __PARAMETERS = "parameters"
    __GENERATOR_CLASS = "generator-class"
    __GENERATOR_SCRIPT = "generator-script"
    __GENERATOR_OPTIONS = "generator-options"

    def __init__(self, name: str, args: Dict[str, Any], parent: IOptions):
        super().__init__(None)
        self.__name = name
        self._parent: IOptions = parent
        self.__args: Dict[str, Any] = args
        self.__test_root = self.__get_arg(self.__TEST_ROOT)
        self.__list_root = self.__get_arg(self.__LIST_ROOT)
        self.__parameters: Dict[str, Any] = args[self.__PARAMETERS] if args and self.__PARAMETERS in args else {}
        self.__expand_macros_in_parameters()

    def __str__(self) -> str:
        return f"{self.__name}: {self._to_str(indent=3)}"

    @cached_property
    def name(self) -> str:
        return self.__name

    @cached_property
    def test_root(self) -> Path:
        return Path(self.__test_root)

    @cached_property
    def list_root(self) -> Path:
        return Path(self.__list_root)

    @cached_property
    def generator_class(self) -> Optional[str]:
        return self.__get_from_args(self.__GENERATOR_CLASS)

    @cached_property
    def generator_script(self) -> Optional[str]:
        return self.__get_from_args(self.__GENERATOR_SCRIPT)

    @cached_property
    def generator_options(self) -> List[str]:
        default_options: List[str] = []
        return cast(List[str], self.__get_from_args(self.__GENERATOR_CLASS, default_options))

    @cached_property
    def parameters(self) -> Dict[str, Any]:
        return self.__parameters

    def get_parameter(self, key: str, default: Optional[Any] = None) -> Optional[Any]:
        return self.__parameters.get(key, default)

    def __get_from_args(self, key: str, default_value: Optional[Any] = None) -> Optional[Any]:
        return self.__args[key] if self.__args and key in self.__args else default_value

    def __get_arg(self, prop_name_minused: str) -> str:
        result = ""
        prop_name_underscored = utils.convert_minus(prop_name_minused)
        if self.__args and prop_name_minused in self.__args:
            result = cast(str, Macros.correct_macro(self.__args[prop_name_minused], self._parent))
        elif prop_name_underscored in self._parent.properties():
            result = cast(str, self._parent.get_value(prop_name_underscored))
            result = cast(str, Macros.correct_macro(result, self._parent))
        elif self.__PARAMETERS in self._parent.properties():
            params = cast(dict, self._parent.get_value(self.__PARAMETERS))
            if prop_name_minused in params:
                result = cast(str, Macros.correct_macro(params[prop_name_minused], self._parent))
        else:
            raise ValueError(f"Unknown key {prop_name_minused}")
        return result

    def __expand_macros_in_parameters(self) -> None:
        for param_name, param_value in self.__parameters.items():
            if isinstance(param_value, str):
                try:
                    self.__parameters[param_name] = Macros.correct_macro(param_value, self)
                except ParameterNotFound:
                    pass
