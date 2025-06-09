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

const ONE = 1;
const TEN = 10;
const TWENTY = 20;
const PRECISION = 3;

import { DynPropertiesWrapper } from './lib1';

function assertEq<T>(a: T, b: T): void {
	// @ts-ignore
	print(`assertEq: '${a}' === '${b}'`);
	if (a !== b) {
		throw new Error(`assertEq failed: '${a}' === '${b}'`);
	}
}

export function main(): void {
	testDynProperties();
}

function testDynProperties(): void {
	const dynPropsWrapper = new DynPropertiesWrapper();
	const KEY_EXTERNAL = 'key_external';
	const VALUE_EXTERNAL = 'value_external';
	const KEY_INTERNAL = 'key_internal';
	const VALUE_INTERNAL = 'value_internal';

	dynPropsWrapper.setPropertyExternal(KEY_EXTERNAL, VALUE_EXTERNAL);
	assertEq(dynPropsWrapper.getPropertyExternal(KEY_EXTERNAL), VALUE_EXTERNAL);

	dynPropsWrapper.dynProps_.SetPropertyInternal(KEY_INTERNAL, VALUE_INTERNAL);
	assertEq(dynPropsWrapper.dynProps_.GetPropertyInternal(KEY_INTERNAL), VALUE_INTERNAL);
}
