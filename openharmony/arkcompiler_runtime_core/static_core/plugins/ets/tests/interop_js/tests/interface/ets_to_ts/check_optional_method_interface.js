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

const {
	num,
	string,
	WithOptionalMethodClass,
	createClassWithOptionalMethod,
	WithoutOptionalMethodClass,
	createClassWithoutOptionalMethod,
	optionalArg,
	optionalArgArray,
} = require('interface_method.test.abc');

function checkWithOptionalMethodClass() {
	const optionalClass = new WithOptionalMethodClass();

	ASSERT_TRUE(optionalClass.getNum() === num && optionalClass.getStr() === string);
}
function checkWithoutOptionalMethodClass() {
	const optionalClass = new WithoutOptionalMethodClass();

	ASSERT_TRUE(optionalClass.getStr() === string);
}

function checkCreateClassWithOptionalMethod() {
	const optionalClass = createClassWithOptionalMethod();

	ASSERT_TRUE(optionalClass.getNum() === num && optionalClass.getStr() === string);
}
function checkCreateClassWithoutOptionalMethod() {
	const optionalClass = createClassWithoutOptionalMethod();

	ASSERT_TRUE(optionalClass.getStr() === string);
}

function checkWithOptionalMethodInstanceClass() {
	ASSERT_TRUE(withOptionalMethodInstanceClass.getNum() === tsNumber && withOptionalMethodInstanceClass.getStr() === tsString);
}

function checkWithoutOptionalMethodInstanceClass() {
	ASSERT_TRUE(withoutOptionalMethodInstanceClass.getStr() === tsString);
}

function checkOptionalArgWithAllArgs() {
	const result = optionalArg(withOptionalMethodInstanceClass, withoutOptionalMethodInstanceClass);
	ASSERT_TRUE(!!result.with && !!result.without);
}

function checkOptionalArgWithOneArgs() {
	const result = optionalArg(withOptionalMethodInstanceClass);

	ASSERT_TRUE(!!result.with);
}

function checkSpreadOperatorArgWithAllArgs() {
	const arr = [withOptionalMethodInstanceClass, withoutOptionalMethodInstanceClass];
	const result = optionalArgArray(...arr);
	ASSERT_TRUE(!!result.with && !!result.without);
}

function checkSpreadOperatorArgWithOneArgs() {
	const arr = [withOptionalMethodInstanceClass];
	const result = optionalArgArray(...arr);
	ASSERT_TRUE(!!result.with);
}
