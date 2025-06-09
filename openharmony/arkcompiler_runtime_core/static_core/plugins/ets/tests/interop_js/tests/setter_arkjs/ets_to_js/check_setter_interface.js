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
const string = 'string';
const InterfaceSetter = etsVm.getClass('Lsetter/test/InterfaceSetter;');
const createInterfaceClass = etsVm.getFunction('Lsetter/test/ETSGLOBAL;', 'create_interface_class');

function checkSetterInterface() {
	const interfaceSetter = new InterfaceSetter();
	interfaceSetter.value = string;

	ASSERT_TRUE(string === interfaceSetter.value);
}

function checkSetterInterfaceClassFromEts() {
	const interfaceSetter = createInterfaceClass();
	interfaceSetter.value = string;

	ASSERT_TRUE(string === interfaceSetter.value);
}

checkSetterInterface();
checkSetterInterfaceClassFromEts();
