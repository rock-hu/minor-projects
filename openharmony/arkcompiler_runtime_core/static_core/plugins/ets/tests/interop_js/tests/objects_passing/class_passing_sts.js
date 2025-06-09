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

const { etsVm, getTestModule } = require('objects_passing/objects_passing_sts.test.abc');

const etsMod = getTestModule('objects_passing_ets_test');
const GCJSRuntimeCleanup = etsMod.getFunction('GCJSRuntimeCleanup');
const getValueObject = etsMod.getFunction('getObjectName');
const getUserName = etsMod.getFunction('getClassUserName');
const getUserAge = etsMod.getFunction('getClassUserAge');
const getUserInfo = etsMod.getFunction('getClassUserInfo');

// Passing class to ets function

class TestUser {
	Constructor(name, age) {
		this.name = name;
		this.age = age;
	}

	showInfo() {
		return `User name: ${this.name}, age: ${this.age}`;
	}
}

const testUser = new TestUser('TestName', 30);

{
	let ret = getUserName(testUser);
	print(ret);
	ASSERT_EQ(ret, 'TestName');
}

{
	let ret = getUserAge(testUser);
	print(ret);
	ASSERT_EQ(ret, 30);
}

{
	let ret = getUserInfo(testUser);
	print(ret);
	ASSERT_EQ(ret, 'User name: TestName, age: 30');
}
