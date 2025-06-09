/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

const etsVm = globalThis.gtest.etsVm;
const num = 1;
const UnionSetter = etsVm.getClass('Lsetter/test/UnionSetter;');
const createUnionClass = etsVm.getFunction('Lsetter/test/ETSGLOBAL;', 'create_union_class');

function checkSetterUnion() {
	const unionSetter = new UnionSetter();
	unionSetter.value = num;

	ASSERT_TRUE(num === unionSetter.value);
}

function checkSetterUnionClassFromEts() {
	const unionSetter = createUnionClass();
	unionSetter.value = num;

	ASSERT_TRUE(num === unionSetter.value);
}

checkSetterUnion();
checkSetterUnionClassFromEts();
