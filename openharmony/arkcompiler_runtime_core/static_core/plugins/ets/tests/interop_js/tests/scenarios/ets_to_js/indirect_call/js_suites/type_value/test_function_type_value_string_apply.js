/**
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
const { etsVm, getTestModule } = require('../../indirect_call.test.abc');

const etsMod = getTestModule('indirect_call_test_type_value');
const GCJSRuntimeCleanup = etsMod.getFunction('GCJSRuntimeCleanup');
const functionTypeValueString = etsMod.getFunction('function_type_value_string');

function testFunctionValueTypeStringApply() {
	const ARG = 'foo';
	const EXPECTED = ''.concat(ARG, ARG);

	let result = functionTypeValueString.apply(null, [ARG]);

	ASSERT_EQ(result, EXPECTED);
}

testFunctionValueTypeStringApply();

GCJSRuntimeCleanup();
