/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

const TWO = 2;

import { NumberOrStringEquality } from './lib';

function assertEq<T>(a: T, b: T): void {
	// @ts-ignore
	print(`assertEq: '${a}' === '${b}'`);
	if (a !== b) {
		throw new Error(`assertEq failed: '${a}' === '${b}'`);
	}
}

export function main(): void {
	testUnions();
}

function testUnions(): void {
	assertEq(NumberOrStringEquality(TWO, TWO), true);
	assertEq(NumberOrStringEquality(TWO, '2'), false);
	assertEq(NumberOrStringEquality('2', '2'), true);
}
