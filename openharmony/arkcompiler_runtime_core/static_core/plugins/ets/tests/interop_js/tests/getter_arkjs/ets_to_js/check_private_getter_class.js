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

import { PRIVATE_GETTER_CLASS_VALUE } from './constant';

const etsVm = globalThis.gtest.etsVm;
const PrivateGetterClass = etsVm.getClass('Lgetter/test/PrivateGetterClass;');
const createPrivateGetterClassFromEts = etsVm.getFunction('Lgetter/test/ETSGLOBAL;', 'create_private_getter_class_from_ets');


function checkrPivateGetterClassalue() {
	const GClass = new PrivateGetterClass();
	ASSERT_TRUE(GClass.alue === undefined);
}

function checkCreatePrivateGetterClassalueFromEts() {
	const GClass = createPrivateGetterClassFromEts();
	ASSERT_TRUE(GClass.alue === undefined);
}

function checkPrivateGetterClass() {
	const GClass = new PrivateGetterClass();
	ASSERT_TRUE(GClass.value === PRIVATE_GETTER_CLASS_VALUE);
}

function checkCreatePrivateGetterlassFromEts() {
	const GClass = createPrivateGetterClassFromEts();
	ASSERT_TRUE(GClass.value === PRIVATE_GETTER_CLASS_VALUE);
}

checkrPivateGetterClassalue();
checkCreatePrivateGetterClassalueFromEts();
checkPrivateGetterClass();
checkCreatePrivateGetterlassFromEts();
