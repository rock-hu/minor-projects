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

import {
	PRIVATE_GETTER_CLASS_VALUE,
	PRIVATE_GETTER_OVERRIDE_CLASS_VALUE
} from './constant';

const etsVm = globalThis.gtest.etsVm;
const PrivateGetterInheritanceClass = etsVm.getClass('Lgetter/test/PrivateGetterInheritanceClass;');
const PrivateGetterOverrideClass = etsVm.getClass('Lgetter/test/PrivateGetterOverrideClass;');
const createPrivateGetterInheritanceClassFromEts = etsVm.getFunction('Lgetter/test/ETSGLOBAL;', 'create_private_getter_inheritance_class_from_ets');
const createPrivateGetterOverrideClassFromEts = etsVm.getFunction('Lgetter/test/ETSGLOBAL;', 'create_private_getter_override_class_from_ets');


function checkPrivateGetterClassInheritancealue() {
	const GClass = new PrivateGetterInheritanceClass();
	ASSERT_TRUE(GClass.alue === undefined);
}

function checkCreatePrivateGetterClassInheritanceFromalueEts() {
	const GClass = createPrivateGetterInheritanceClassFromEts();
	ASSERT_TRUE(GClass.alue === undefined);
}

function checkPrivateGetterClassInheritance() {
	const GClass = new PrivateGetterInheritanceClass();
	ASSERT_TRUE(GClass.value === PRIVATE_GETTER_CLASS_VALUE);
}

function checkCreatePrivateetterClassInheritanceFromEts() {
	const GClass = createPrivateGetterInheritanceClassFromEts();
	ASSERT_TRUE(GClass.value === PRIVATE_GETTER_CLASS_VALUE);
}

function checkPrivateGetterClassOverride() {
	const GClass = new PrivateGetterOverrideClass();
	ASSERT_TRUE(GClass.value === PRIVATE_GETTER_OVERRIDE_CLASS_VALUE);
}

function checkCreatePrivateetterClassOverrideFromEts() {
	const GClass = createPrivateGetterOverrideClassFromEts();
	ASSERT_TRUE(GClass.value === PRIVATE_GETTER_OVERRIDE_CLASS_VALUE);
}

checkPrivateGetterClassInheritancealue();
checkCreatePrivateGetterClassInheritanceFromalueEts();
checkPrivateGetterClassInheritance();
checkCreatePrivateetterClassInheritanceFromEts();
checkPrivateGetterClassOverride();
checkCreatePrivateetterClassOverrideFromEts();