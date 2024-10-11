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
const CreateEtsSampleWithCause = etsMod.getFunction('Error_CreateEtsSampleWithCause');
const TestJSWithCause = etsMod.getFunction('Error_TestJSWithCause');

{
	// Test JS Error
	TestJSWithCause(new Error('message', 'cause'));
}

{
	// Test ETS Error
	let v = CreateEtsSampleWithCause();
	ASSERT_TRUE(v instanceof Error);

	ASSERT_EQ(String(v.message), 'message');

	ASSERT_EQ(String(v.cause), 'cause');

	ASSERT_TRUE(String(v['<get>stack']()).includes('CreateEtsSampleWithCause'));
}
