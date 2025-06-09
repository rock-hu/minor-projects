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
const { etsVm, getTestModule } = require('../scenarios.test.abc');

const etsMod = getTestModule('scenarios_test');
const GCJSRuntimeCleanup = etsMod.getFunction('GCJSRuntimeCleanup');

let etsSumRestParams = etsMod.getFunction('etsSumRestParams');
let etsMultiplyArgBySumRestParams = etsMod.getFunction('etsMultiplyLargBySumRestParams');
let etsMultiplySumArgsBySumRestParams = etsMod.getFunction('etsMultiplySum2argsBySumRestParams');
let etsConcatStringsRestParams = etsMod.getFunction('etsConcatStringsRestParams');
let etsMethodRestParams = etsMod.getFunction('etsCallFooRestParams');
let F = etsMod.getClass('RestParamsTest');
{
	ASSERT_EQ(etsSumRestParams(1, 2, 3), 1 + 2 + 3);
	ASSERT_EQ(etsMultiplyArgBySumRestParams(1, 2, 3, 4), 1 * (2 + 3 + 4));
	ASSERT_EQ(etsMultiplySumArgsBySumRestParams(1, 2, 3, 4, 5), (1 + 2) * (3 + 4 + 5));
	ASSERT_EQ(etsConcatStringsRestParams(), '');
	ASSERT_EQ(etsConcatStringsRestParams('a', 'b', 'c', 'd'), 'abcd');
	ASSERT_EQ(etsMethodRestParams(new F(), new F(), new F()), 9);
	let foo = new F();
	ASSERT_EQ(foo.sumInts(1, 2, 3), 1 + 2 + 3);
}

GCJSRuntimeCleanup();
