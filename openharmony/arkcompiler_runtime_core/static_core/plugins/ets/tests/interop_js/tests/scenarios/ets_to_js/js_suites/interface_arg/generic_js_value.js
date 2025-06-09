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
const unpackValue = etsMod.getFunction('unpack_value');

class Boxx {
	constructor(value) {
		this.value_ = value;
	}

	getValue() {
		return this.value_;
	}
}

{
	const INT_VALUE = 222;
	const STRING_VALUE = 'hehehe';

	const intBox = new Boxx(INT_VALUE);
	const stringBox = new Boxx(STRING_VALUE);

	let intRet = unpackValue(intBox);
	ASSERT_EQ(intRet, INT_VALUE);

	let stringRet = unpackValue(stringBox);
	ASSERT_EQ(stringRet, STRING_VALUE);
}
