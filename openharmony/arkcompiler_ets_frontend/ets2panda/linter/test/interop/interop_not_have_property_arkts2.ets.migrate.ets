/*
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

'use static'
let GeneratedImportVar_1 = ESValue.load('./interop_not_have_property_js');
let foo = GeneratedImportVar_1.getProperty('foo');
let person = GeneratedImportVar_1.getProperty('person');
let TestHelper = GeneratedImportVar_1.getProperty('TestHelper');
let Machine = GeneratedImportVar_1.getProperty('Machine');
let User = GeneratedImportVar_1.getProperty('User');
let Person = GeneratedImportVar_1.getProperty('Person');
let Employee = GeneratedImportVar_1.getProperty('Employee');


foo.getProperty("name")
foo.setProperty("name", ESValue.wrap("456"))
person.setProperty("age", ESValue.wrap(23.0))
person.setProperty("male", ESValue.wrap([2.0, 3.0]))
foo.setProperty("age", ESValue.wrap(12.0))
if (foo.setProperty("name", ESValue.wrap("456"))) { print("true") }

let a = foo.instantiate()
a.setProperty("age", ESValue.wrap(12.0))

let test_helper = TestHelper.instantiate(ESValue.wrap("TEST_INSTANTIATE_JS_OBJECT"));
test_helper.invokeMethod("test", ESValue.wrap(() => {
    let machine = new Machine();
    return machine.name === "machine"; // arkts-interop-js2s-access-js-prop
}), ESValue.wrap("machine.name === 'machine'"));

test_helper.invokeMethod("test", ESValue.wrap(() => {
    let user = new User("Bob");
    return user.id === "Bob"; // arkts-interop-js2s-access-js-prop
}), ESValue.wrap("user.id === 'Bob'"));

test_helper.invokeMethod("test", ESValue.wrap(() => {
let user = new User(10.0);
return user.id === 10.0;// arkts-interop-js2s-access-js-prop
}), ESValue.wrap("user.id === 10"));

test_helper.invokeMethod("test", ESValue.wrap(() => {
    let user = new User(123n);
    return user.id === 123n; // arkts-interop-js2s-access-js-prop
}), ESValue.wrap("user.id === 123n"));

test_helper.invokeMethod("test", ESValue.wrap(() => {
    let user = new User(true);
    return user.id === true;// arkts-interop-js2s-access-js-prop
}), ESValue.wrap("user.id === true"));

test_helper.invokeMethod("test", ESValue.wrap(() => {
    let machine = new Person("John", 10.0);
    return machine.name === "machine"; // arkts-interop-js2s-access-js-prop
}), ESValue.wrap("machine.name === 'machine'"));

test_helper.invokeMethod("test", ESValue.wrap(() => {
    let employee = new Employee();
    return employee.name === "employee"; // arkts-interop-js2s-access-js-prop
}), ESValue.wrap("employee.name === 'employee'"));