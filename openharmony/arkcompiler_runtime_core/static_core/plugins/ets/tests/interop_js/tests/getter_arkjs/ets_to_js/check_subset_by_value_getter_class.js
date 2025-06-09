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

import { PUBLIC_GETTER_CLASS_VALUE } from './constant';

const etsVm = globalThis.gtest.etsVm;
const PublicGetterClass = etsVm.getClass('Lgetter/test/PublicGetterClass;');
const SubsetByValueClass = etsVm.getClass('Lgetter/test/SubsetByValueClass;');
const createSubsetByValueGetterClassFromEts = etsVm.getFunction('Lgetter/test/ETSGLOBAL;', 'create_subset_by_value_getter_class_from_ets');


function checkSubsetByValueGetterClass() {
	const GClass = new SubsetByValueClass(new PublicGetterClass().value);
	ASSERT_TRUE(GClass.value === PUBLIC_GETTER_CLASS_VALUE);
}

function checkSubsetByValueGetteralueClass() {
	const GClass = new SubsetByValueClass(new PublicGetterClass().value);
	ASSERT_TRUE(GClass.alue === undefined);
}

function checkCreateSubsetByValueGetteralueClassFromEts() {
	const GClass = createSubsetByValueGetterClassFromEts();
	ASSERT_TRUE(GClass.alue === undefined);
}

function checkCreateSubsetByValueGettervClassFromEts() {
	const GClass = createSubsetByValueGetterClassFromEts();
	ASSERT_TRUE(GClass.value === PUBLIC_GETTER_CLASS_VALUE);
}

checkSubsetByValueGetterClass();
checkSubsetByValueGetteralueClass();
checkCreateSubsetByValueGetteralueClassFromEts();
checkCreateSubsetByValueGettervClassFromEts();
