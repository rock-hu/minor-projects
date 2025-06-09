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

const etsFnName = 'fnWithArgOfClassWithSymbolProperty';
const etsSym = getTestModule('scenarios_test');
const fnWithArgOfClassWithSymbolProperty = etsSym.getFunction(etsFnName);
const ClassToExtendEts = etsSym.getClass('ClassToExtendEts');

let testProperty = Symbol.for('id');

class ChildClass extends ClassToExtendEts {
  constructor(...args) {
    super(...args);
    this[testProperty] = 1;
  }
}

{
  let testClass = new ChildClass();
  let expectedClass = fnWithArgOfClassWithSymbolProperty(testClass);
  ASSERT_EQ(expectedClass[testProperty], 1);
  ASSERT_EQ(expectedClass.x, 1);
}
