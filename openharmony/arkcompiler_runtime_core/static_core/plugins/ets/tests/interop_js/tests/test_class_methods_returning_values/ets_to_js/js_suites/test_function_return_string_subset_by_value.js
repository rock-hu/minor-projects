/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
const { getTestModule } = require('test_class_methods.js');

const etsMod = getTestModule('class_methods_test');
const functionReturnStrEts = etsMod.getFunction('functionReturnStringSubsetByValue');

{
	const EXPECTED_STRING = 'This is a test string';
	const testVal = functionReturnStrEts();

	ASSERT_EQ(testVal, EXPECTED_STRING);
	ASSERT_EQ(typeof testVal, 'string');
}
