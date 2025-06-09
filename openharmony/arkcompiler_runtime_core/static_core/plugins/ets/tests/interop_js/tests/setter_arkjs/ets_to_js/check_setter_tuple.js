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
const string = 'string';
const TupleSet = etsVm.getClass('Lsetter/test/TupleSet;');
const createTupleSetClass = etsVm.getFunction('Lsetter/test/ETSGLOBAL;', 'create_tuple_set_class');

function checkSetterTuple() {
	const tupleSet = new TupleSet();
	const arr = [num, string];
	tupleSet.value = arr;

	ASSERT_TRUE(arr[0] === tupleSet.value[0]);
	ASSERT_TRUE(arr[1] === tupleSet.value[1]);
}

function checkSetterTupleClassFromEts() {
	const tupleSet = createTupleSetClass();
	const arr = [num, string];
	tupleSet.value = arr;

	ASSERT_TRUE(arr[0] === tupleSet.value[0]);
	ASSERT_TRUE(arr[1] === tupleSet.value[1]);
}

checkSetterTuple();
checkSetterTupleClassFromEts();
