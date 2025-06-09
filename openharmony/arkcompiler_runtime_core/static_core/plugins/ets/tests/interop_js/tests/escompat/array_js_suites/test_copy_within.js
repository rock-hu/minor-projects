/**
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

const { etsVm, getTestModule } = require('../escompat.test.abc');

const etsMod = getTestModule('escompat_test');
const GCJSRuntimeCleanup = etsMod.getFunction('GCJSRuntimeCleanup');
const FooClass = etsMod.getClass('FooClass');
const CreateEtsSample = etsMod.getFunction('Array_CreateEtsSample');
const TestJSCopyWithin = etsMod.getFunction('Array_TestJSCopyWithin');

// NOTE (oignatenko) enable when arr.at is fully supported
const FIXES_IMPLEMENTED = false;
{
	// Test JS Array<FooClass>
	TestJSCopyWithin(new Array(new FooClass('zero'), new FooClass('one')));
}

{
	// Test ETS Array<Object>
	let arr = CreateEtsSample();
	arr.push('something');
	ASSERT_EQ(arr.at(2), 'something');
	arr.copyWithin(2, 0);
	ASSERT_EQ(arr.at(2), 123);

	ASSERT_EQ(arr.at(1), 'foo');
	if (FIXES_IMPLEMENTED) {
		arr.copyWithin(1);
		ASSERT_EQ(arr.at(1), 123);
	}
}

GCJSRuntimeCleanup();
