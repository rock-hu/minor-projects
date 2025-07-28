#!/usr/bin/env python3
# -- coding: utf-8 --
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
from pathlib import Path
from unittest import TestCase
from unittest.mock import MagicMock, patch

from runner.init_runner import InitRunner
from runner.test.test_utils import random_suffix

non_existing_path_01: str = str(Path(__file__).parent / "non-exist-01")
existing_path_01: str = str(Path(__file__).parent / "init-exist-01")
existing_path_02: str = str(Path(__file__).parent / "init-exist-02")
existing_path_03: str = str(Path(__file__).parent / "init-exist-03")
existing_path_04: str = str(Path(__file__).parent / "init-exist-04")
existing_path_05: str = str(Path(__file__).parent / "init-exist-05")
existing_path_06: str = str(Path(__file__).parent / "init-exist-06")


class InitRunnerTest(TestCase):
    def setUp(self) -> None:
        global_env = f".urunner.{random_suffix()}"
        self.global_env_path = Path(__file__).with_name(global_env)

    def test_is_runner_initialized_exist(self) -> None:
        # preparation
        self.global_env_path.touch(exist_ok=True)
        # test
        init_runner = InitRunner(self.global_env_path)
        self.assertTrue(init_runner.is_runner_initialized())
        # clear up
        self.global_env_path.unlink(missing_ok=True)

    def test_is_runner_initialized_not_exist(self) -> None:
        # preparation
        self.global_env_path.unlink(missing_ok=True)
        # test
        init_runner = InitRunner(self.global_env_path)
        self.assertFalse(init_runner.is_runner_initialized())

    def test_should_runner_initialize_yes1(self) -> None:
        self.assertTrue(InitRunner.should_runner_initialize(["runner.sh", "--init"]))

    def test_should_runner_initialize_yes2(self) -> None:
        cli_options = ["runner.sh", "es2panda", "ets-cts", "--init", "--show-progress"]
        self.assertTrue(InitRunner.should_runner_initialize(cli_options))

    def test_should_runner_initialize_no(self) -> None:
        self.assertFalse(InitRunner.should_runner_initialize(["runner.sh", "only-es2panda", "ets-cts"]))

    @patch('builtins.input', return_value=existing_path_01)
    def test_input_value_must_exist_exist(self, _: MagicMock) -> None:
        # preparation
        prop_path = Path(existing_path_01)
        prop_path.mkdir(parents=True, exist_ok=True)
        # test
        actual = InitRunner.input_value("prop", must_exist=True, attempts=1)
        self.assertEqual(actual, existing_path_01)
        # clear up
        prop_path.rmdir()

    @patch('builtins.input', return_value=non_existing_path_01)
    def test_input_value_must_exist_absent(self, _: MagicMock) -> None:
        # preparation
        prop_path = Path(non_existing_path_01)
        if prop_path.exists():
            prop_path.rmdir()
        # test
        self.assertRaises(
            FileNotFoundError,
            InitRunner.input_value,
            "prop", True, 1
        )

    @patch('builtins.input', return_value=non_existing_path_01)
    def test_input_value_can_exist_absent(self, _: MagicMock) -> None:
        # preparation
        prop_path = Path(non_existing_path_01)
        if prop_path.exists():
            prop_path.rmdir()
        # test
        actual = InitRunner.input_value("prop", must_exist=False, attempts=1)
        self.assertEqual(actual, non_existing_path_01)

    @patch('builtins.input', return_value=existing_path_02)
    def test_input_value_can_exist_exist(self, _: MagicMock) -> None:
        # preparation
        prop_path = Path(existing_path_02)
        prop_path.mkdir(parents=True, exist_ok=True)
        # test
        actual = InitRunner.input_value("prop", must_exist=False, attempts=1)
        self.assertEqual(actual, existing_path_02)
        # clear up
        prop_path.rmdir()

    @patch('builtins.input', return_value=non_existing_path_01)
    def test_input_value_repeats_1(self, input_value: MagicMock) -> None:
        # preparation
        prop_path = Path(non_existing_path_01)
        if prop_path.exists():
            prop_path.rmdir()
        # test
        self.assertRaises(
            FileNotFoundError,
            InitRunner.input_value,
            "prop", True, 2
        )
        self.assertEqual(2, input_value.call_count)

    @patch('builtins.input', side_effect=["", existing_path_04])
    def test_input_value_repeats_2(self, input_value: MagicMock) -> None:
        # preparation
        prop_path = Path(existing_path_04)
        prop_path.mkdir(parents=True, exist_ok=True)
        # test
        actual = InitRunner.input_value("prop", must_exist=False, attempts=2)
        self.assertEqual(actual, existing_path_04)
        self.assertEqual(2, input_value.call_count)

    @patch('builtins.input', side_effect=[non_existing_path_01, existing_path_05])
    def test_input_value_repeats_3(self, input_value: MagicMock) -> None:
        # preparation
        prop_path1 = Path(non_existing_path_01)
        if prop_path1.exists():
            prop_path1.rmdir()
        prop_path2 = Path(existing_path_05)
        prop_path2.mkdir(parents=True, exist_ok=True)
        # test
        actual = InitRunner.input_value("prop", must_exist=True, attempts=2)
        self.assertEqual(actual, existing_path_05)
        self.assertEqual(2, input_value.call_count)
        # clear up
        prop_path2.rmdir()

    def test_init_property_value_not_none_can_exist_absent(self) -> None:
        # test
        runner = InitRunner()
        actual = runner.init_property("prop", "value", must_exist=False)
        self.assertEqual(actual, "value")

    def test_init_property_value_not_none_must_exist_exist(self) -> None:
        # preparation
        prop_path = Path(existing_path_03)
        prop_path.mkdir(parents=True, exist_ok=True)
        # test
        runner = InitRunner()
        actual = runner.init_property("prop", existing_path_03, must_exist=True)
        self.assertEqual(actual, existing_path_03)
        # clear up
        prop_path.rmdir()

    @patch('builtins.input', side_effect=["", non_existing_path_01, existing_path_06])
    def test_init_property_value_none_must_exist_exist(self, input_value: MagicMock) -> None:
        # preparation
        prop_path = Path(existing_path_06)
        prop_path.mkdir(parents=True, exist_ok=True)
        # test
        runner = InitRunner()
        actual = runner.init_property("prop", None, must_exist=True)
        self.assertEqual(actual, existing_path_06)
        self.assertEqual(3, input_value.call_count)
        # clear up
        prop_path.rmdir()
