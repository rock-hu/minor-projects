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

export class UnionSetter {
    set value(arg) {
        this._value = arg;
    }
    get value() {
        return this._value;
    }
}
export class InterfaceSetter {
    set value(arg) {
        this._value = arg;
    }
    get value() {
        return this._value;
    }
}
export class BaseClass {
    set value(arg) {
        this._value = arg;
    }
    get value() {
        return this._value;
    }
}
export class SubsetRefSet extends BaseClass {
}
export const tsTestString = 'ts_test_string';
class ValueSetter {
    constructor() {
        this._value = tsTestString;
    }
}
export class SubsetValueSet extends ValueSetter {
    constructor() {
        super();
    }
    set value(arg) {
        this._value = arg;
    }
    get value() {
        return this._value;
    }
}
export class SetterAnyType {
    set value(arg) {
        this._value = arg;
    }
    get value() {
        return this._value;
    }
}
export class TupleSet {
    set value(arg) {
        this._value = arg;
    }
    get value() {
        return this._value;
    }
}
class AbstractSetter {
}
export class AbstractClass extends AbstractSetter {
    set value(arg) {
        this._value = arg;
    }
    get value() {
        return this._value;
    }
}
export const AbstractClassObject = new AbstractClass();
export const UnionSetterObject = new UnionSetter();
export const InterfaceSetterObject = new InterfaceSetter();
export const TupleSetObject = new TupleSet();
export const SetterAnyTypeObject = new SetterAnyType();
export const BaseClassObject = new BaseClass();
export const SubsetRefSetObject = new SubsetRefSet();
export const SubsetValueSetObject = new SubsetValueSet();
