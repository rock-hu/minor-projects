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

class A {
    _field: Int = 30;
    _bfield: boolean = true;
}

get field(this: A): Int {
    return this._field;
}

set field(this: A, a: int) {
    this._field = a;
}

get bfield(this: A): boolean {
    return this._bfield;
}

set bfield(this: A, b: boolean) {
    this._bfield = b;
}

function main(): void {
    let obj: A = new A();
    assertEQ(obj.field, 30)
    obj.field = 29;
    assertEQ(obj.field, 29)
    obj.field = 15 + 15;

    let z = obj.field + obj.field;
    assertEQ(z, 60)
    z = obj.field + obj.field + obj.field;
    assertEQ(z, 90)
    let u = obj.field + 10;
    assertEQ(u, 40)
    let i = 10 + obj.field;
    assertEQ(i, 40)
    u = 10 + obj.field + obj.field + 10;
    obj.field =  10 + obj.field + obj.field + 10;
    assertEQ(u, 80)
    assertEQ(obj.field, 80)

    obj.field = 10;
    let k = obj.field;
    assertEQ(k, 10)

    obj.field = obj.field; 
    k = obj.field;
    assertEQ(k, 10)

    obj.field = obj.field + obj.field;
    k = obj.field;
    assertEQ(obj.field, 20)

    obj.field = obj.field + 10;
    k = obj.field;
    assertEQ(k, 30)

    obj.field = 10 + obj.field;
    k = obj.field;
    assertEQ(k, 40)

    obj.field = 10 * obj.field;
    k = obj.field;
    assertEQ(k, 400)

    obj.field = obj.field / 2;
    k = obj.field;
    assertEQ(k, 200)

    obj.field = 1;
    obj.field = obj.field & 0;
    k = obj.field;
    assertEQ(k, 0)

    obj.bfield = true;
    obj.bfield = obj.bfield & false;
    let boolk = obj.bfield;
    assertEQ(boolk, false)

    obj.bfield = true;
    obj.bfield = obj.bfield | false;
    boolk = obj.bfield;
    assertEQ(boolk, true)

    obj.bfield = true;
    obj.bfield = obj.bfield | obj.bfield;
    boolk = obj.bfield;
    assertEQ(boolk, true)
}
