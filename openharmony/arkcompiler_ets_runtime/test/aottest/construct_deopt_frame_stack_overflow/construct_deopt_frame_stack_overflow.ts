/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

function bgc()
{
	for (let i = 0; i < 0x10000; i += 1) {
		new String();
	}

	let a = new Array(0x10000);
	for (let i = 0; i < 0x10000; i += 1) {
		a[i] = new Array(0x100);
	}
}

function sgc()
{
	for (let i = 0; i < 0x10000; i += 1) {
		new String();
	}
}

function runNearStackLimit(f)
{
	function t()
	{
		try {
			t();
		} catch (e) {
			f();
		}
	};

	try {
		t();
	} catch (e) {}
}

function f24()
{
	Math.log1p(f24);
	return Math;
}

runNearStackLimit(f24);
