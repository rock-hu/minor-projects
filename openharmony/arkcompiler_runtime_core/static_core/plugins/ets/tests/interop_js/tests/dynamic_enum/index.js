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
'use strict';

export function makeEnum(arr) {
	let obj = Object.create(null);
	for (let val of arr) {
		obj[val] = Symbol(val);
	}
	return Object.freeze(obj);
}

export function jsEnum() {
	const Direction = {
		Up: 'Up',
		Down: 'Down',
		Left: 'Left',
		Right: 'Right',
	};
	return Direction;
}

export function isEq(val1, val2) {
	if (val1 === val2) {
		return true;
	}
	return false;
}