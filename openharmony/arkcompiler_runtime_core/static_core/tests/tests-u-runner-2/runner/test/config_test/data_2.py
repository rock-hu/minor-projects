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

from runner.enum_types.configuration_kind import ArchitectureKind, BuildTypeKind, OSKind, SanitizerKind
from runner.enum_types.qemu import QemuKind
from runner.enum_types.verbose_format import VerboseFilter, VerboseKind

args = {
    'workflow': 'config-1',
    'config-1.path': Path(__file__).parent.parent.parent.parent.joinpath('cfg/workflows/config-1.yaml').resolve(),
    'config-1.data': {
        'runner-api-level': '0.0.0.1',
        'type': 'workflow',
        'workflow-name': 'echo',
        'parameters': {
            'string-param': 'hello',
            'int-param': 20,
            'bool-param': True,
            'list-param': ['--gen-stdlib=false', '--thread=0']
        },
        'steps': {
            'echo': {
                'executable-path': '/usr/bin/echo',
                'args': [
                    '--arktsconfig=${parameters.string-param}',
                    '${parameters.list-param}',
                    '--extension=ets',
                    '${test-id}'
                ],
                'timeout': '${parameters.int-param}',
                'can-be-instrumented': '${parameters.bool-param}'
            }
        }
    },
    'config-1.parameters.string-param': 'hello',
    'config-1.parameters.int-param': 20,
    'config-1.parameters.bool-param': True,
    'config-1.parameters.list-param': ['--gen-stdlib=false', '--thread=0'],
    'test-suite': 'test_suite1',
    'test_suite1.path': (Path(__file__).parent.parent.parent.parent
                         .joinpath('cfg/test-suites/test_suite1.yaml').resolve()),
    'test_suite1.data': {
        'version': '0.0.0.1',
        'type': 'test-suite',
        'suite-name': 'test_suite',
        'test-root': '.',
        'list-root': '.',
        'parameters': {
            'extension': 'sts',
            'load-runtimes': 'ets',
            'work-dir': '.'
        }
    },
    'test_suite1.parameters.filter': '*',
    'test_suite1.parameters.repeats': 1,
    'test_suite1.parameters.repeats-by-time': 0,
    'test_suite1.parameters.with-js': 'with-js',
    'test_suite1.parameters.test-list': None,
    'test_suite1.parameters.test-file': None,
    'test_suite1.parameters.skip-test-lists': False,
    'test_suite1.parameters.update-excluded': False,
    'test_suite1.parameters.update-expected': False,
    'test_suite1.parameters.test-list-arch': ArchitectureKind.AMD64,
    'test_suite1.parameters.test-list-san': SanitizerKind.NONE,
    'test_suite1.parameters.test-list-os': OSKind.LIN,
    'test_suite1.parameters.test-list-build': BuildTypeKind.FAST_VERIFY,
    'test_suite1.parameters.force-generate': False,
    'test_suite1.parameters.compare-files': False,
    'test_suite1.parameters.compare-files-iterations': 2,
    'test_suite1.parameters.groups': 1,
    'test_suite1.parameters.group-number': 1,
    'test_suite1.parameters.chapters': None,
    'test_suite1.parameters.chapters-file': 'chapters.yaml',
    'test_suite1.parameters.extension': 'sts',
    'test_suite1.parameters.load-runtimes': 'ets',
    'test_suite1.parameters.work-dir': '.',
    'runner.processes': 12,
    'runner.detailed_report': True,
    'runner.detailed_report_file': 'my-report',
    'runner.report_dir': 'my-report-dir',
    'runner.show_progress': True,
    'runner.verbose': VerboseKind.SHORT,
    'runner.verbose_filter': VerboseFilter.IGNORED,
    'runner.qemu': QemuKind.ARM64,
    'runner.enable_time_report': True,
    'runner.time_edges': [1, 10, 100, 500],
    'runner.use_llvm_cov': True,
    'runner.profdata_files_dir': Path.cwd().resolve().as_posix(),
    'runner.coverage_html_report_dir': Path.cwd().resolve().as_posix()
}
