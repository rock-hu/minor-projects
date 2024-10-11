#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Huawei Device Co., Ltd.
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

from vmb.tool import ToolBase, OptFlags, VmbToolExecError
from vmb.target import Target
from vmb.unit import BenchUnit


class Tool(ToolBase):

    def __init__(self, *args) -> None:
        super().__init__(*args)
        self.sdk = self.ensure_dir_env('OHOS_SDK')
        if Target.HOST == self.target:
            self.ark_js_vm = self.ensure_file(
                self.sdk, 'out/x64.release/arkcompiler/ets_runtime/ark_js_vm')
        elif Target.OHOS == self.target:
            self.ark_js_vm = f'{self.dev_dir}/ark_js_vm'
        else:
            raise NotImplementedError(f'Wrong target: {self.target}!')

    @property
    def name(self) -> str:
        return 'Ark Js VM'

    def exec(self, bu: BenchUnit) -> None:
        name = bu.src('.abc').with_suffix('').name
        bu_path = bu.path if self.target == Target.HOST \
            else bu.device_path
        aot = f'--aot-file={name} ' if OptFlags.AOT in self.flags else ''
        res = self.x_run(
            f'{self.ark_js_vm} --entry-point={name} '
            f'{aot}{self.custom} ./{name}.abc',
            cwd=str(bu_path))
        bu.parse_run_output(res)

    def profile(self, bu: BenchUnit, with_aot: bool = False) -> None:
        name = bu.src('.abc').with_suffix('').name
        cwd = bu.path if Target.HOST == self.target else bu.device_path
        aot = '' if not with_aot else f'--aot-file={name} '
        ark_cmd = (
            f'{self.ark_js_vm} --log-level=info '
            '--enable-pgo-profiler=true --asm-interpreter=true '
            f'--compiler-pgo-profiler-path={name}.ap '
            f'--entry-point={name} '
            f'{self.custom} {aot}'
            f'{name}.abc'
        )
        res = self.x_run(ark_cmd, cwd=str(cwd))
        if res.ret != 0:
            raise VmbToolExecError(f'{self.name} failed', res)
        ap = bu.path.joinpath(name).with_suffix('.ap')
        if ap not in bu.binaries:
            bu.binaries.append(ap)

    def kill(self) -> None:
        self.x_sh.run('pkill ark_js_vm')
