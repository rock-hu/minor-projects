/**
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

const { etsVm, getTestModule } = require('escompat.test.js');

const etsMod = getTestModule('escompat_test');
const GCJSRuntimeCleanup = etsMod.getFunction('GCJSRuntimeCleanup');
const FooClass = etsMod.getClass('FooClass');
const CreateEtsSample = etsMod.getFunction('Array_CreateEtsSample');
const TestJSFindLast = etsMod.getFunction('Array_TestJSFindLast');

{
	// Test JS Array<FooClass>
	TestJSFindLast(new Array(new FooClass('zero'), new FooClass('one')));
}

{
	// Test ETS Array<Object>
	let arr = CreateEtsSample();
	function fnTrue(v) {
		return true;
	}
	function fnFalse(v) {
		return false;
	}

	let last = arr.findLast(fnTrue);
	ASSERT_EQ(last, 'foo');
	let lastStatic = Array.findLast(fnTrue, arr);
	ASSERT_EQ(lastStatic, 'foo');

	let lastNull = arr.findLast(fnFalse);
	ASSERT_TRUE(lastNull === null);
	let lastNullStatic = Array.findLast(fnFalse, arr);
	ASSERT_TRUE(lastNullStatic === null);
}

GCJSRuntimeCleanup();
