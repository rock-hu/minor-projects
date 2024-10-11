/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

const ONE_TENTH = 0.1;
const TWO_TENTH = 0.2;
const THREE_TENTH = 0.3;
const FOUR_TENTH = 0.4;
const FIVE_TENTH = 0.5;
const ONE = 1;
const TWO = 2;
const THREE = 3;
const FOUR = 4;
const FIVE = 5;
const SIX = 6;
const PRECISION = 3;

import { Fields } from './lib';

function assertEq<T>(a: T, b: T): void {
	console.log(`assertEq: '${a}' === '${b}'`);
	if (a !== b) {
		throw new Error(`assertEq failed: '${a}' === '${b}'`);
	}
}

export function main(): void {
	testFields();
}

function testFields(): void {
	const fields = new Fields();
	assertEq(fields.readonlyBoolean_, true);
	assertEq(Fields.staticFloat_.toFixed(PRECISION), (ONE + ONE_TENTH).toFixed(PRECISION));
	assertEq(fields.number_.toFixed(PRECISION), (TWO + TWO_TENTH).toFixed(PRECISION));
	assertEq(fields.Number_.toFixed(PRECISION), (THREE + THREE_TENTH).toFixed(PRECISION));
	assertEq(fields.double_.toFixed(PRECISION), (FOUR + FOUR_TENTH).toFixed(PRECISION));
	assertEq(fields.Double_.toFixed(PRECISION), (FIVE + FIVE_TENTH).toFixed(PRECISION));
	assertEq(fields.int_, SIX);
	// NOTE(ivagin): enable when supported by interop #12808
	if (false) {
		assertEq(fields.object_ instanceof Object, true);
		assertEq(fields.rangeError_ instanceof RangeError, true);
		assertEq(fields.uint8Array_ instanceof Int8Array, true);
		assertEq(fields.uint16Array_ instanceof Int16Array, true);
		assertEq(fields.uint32Array_ instanceof Int32Array, true);
	}

	assertEq(fields.string_, 'Panda string!!');
	assertEq(fields.nullableString_, 'nullable string');
	fields.nullableString_ = null;
	assertEq(fields.nullableString_, null);
}
