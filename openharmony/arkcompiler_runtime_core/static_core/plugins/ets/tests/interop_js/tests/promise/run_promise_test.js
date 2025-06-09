/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const helper = requireNapiPreview('libinterop_test_helper.so', false);

function runTest(test) {
	print('Running test ' + test);
	const gtestAbcPath = helper.getEnvironmentVar('ARK_ETS_INTEROP_JS_GTEST_ABC_PATH');
	const stdlibPath = helper.getEnvironmentVar('ARK_ETS_STDLIB_PATH');
	const packageName = helper.getEnvironmentVar('PACKAGE_NAME');
	if (!packageName) {
		throw Error('PACKAGE_NAME is not set');
	}
	const globalName = 'L' + packageName + '/ETSGLOBAL;';

	let etsVm = requireNapiPreview('ets_interop_js_napi.so', false);
	const etsOpts = {
		'panda-files': gtestAbcPath,
		'boot-panda-files': `${stdlibPath}:${gtestAbcPath}`,
		'coroutine-enable-external-scheduling': 'true',
		'xgc-trigger-type': 'never',
	};
	if (!etsVm.createRuntime(etsOpts)) {
		throw Error('Cannot create ETS runtime');
	}
	const runTestImpl = etsVm.getFunction(globalName, test);
	let res = runTestImpl();
	let tId = 0;
	let checkFn = () => {
		const is_unset = etsVm.getFunction(globalName, 'is_unset');
		if (is_unset()) {
			return;
		}
		helper.clearInterval(tId);
		const check = etsVm.getFunction(globalName, 'check');
		check();
	};
	tId = helper.setInterval(checkFn, 0);
}

let args = helper.getArgv();
if (args.length !== 6) {
	throw Error('Expected test name');
}
runTest(args[5]);
