/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

function main(): void {
    assertEQ(foo1(), 10)
    assertEQ(foo2(), c'a')
    assertEQ(foo3(), true)

    assertEQ(foo4(), 30)
    assertEQ(foo4(5), 25)

    assertEQ(foo5(5), 55)
    assertEQ(foo5(5, 10), 45)


    assertEQ(foo6(), undefined)
    assertEQ(foo7(), undefined)
    assertEQ(foo8(), undefined)


    assertEQ(foo13(10), 15)
    assertEQ(foo14(10), 25)

    assertEQ(foo15(10, 5), 20)
    assertEQ(foo16(10, 5), 30)
}

function foo1(a: int = 10): int {
    return a;
}

function foo2(a: char = c'a'): char {
    return a;
}

function foo3(a: boolean = true): boolean {
    return a;
}

function foo4(a: int = 10, b: int = 20): int {
    return a + b;
}

function foo5(a: int = 10, b: int = 20, c: int = 30): int {
    assertEQ(a, 5)

    return a + b + c;
}

function foo6(a?: int): int | undefined {
    return a;
}

function foo7(a?: boolean): boolean | undefined {
    return a;
}

function foo8(a?: char): char | undefined {
    return a;
}

function foo13(a: int, b: int = 5): int {
    return a + b;
}

function foo14(a: int, b: int = 5, c: int = 10): int {
    return a + b + c;
}

function foo15(a: int, b: int, c: int = 5): int {
    return a + b + c;
}

function foo16(a: int, b: int, c: int = 10, d: int = 5): int {
    return a + b + c + d;
}
