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
const { etsVm, getTestModule } = require('../../scenarios.test.abc');

const etsMod = getTestModule('scenarios_test');
const functionArgIsInterface = etsMod.getFunction('function_arg_is_interface');

{
	const INT_VALUE = 222;

	// Interface requires to have callable 'foo' field, but integer value given
	ASSERT_THROWS(TypeError, () =>
		functionArgIsInterface({
			foo: 123,
			numVal: INT_VALUE,
			stringVal: 'hehehe',
		})
	);
}
