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

interface InterfaceA {
    propA: string
}
interface InterfaceAA extends InterfaceA {}

interface InterfaceB {
    propB: number;
}
interface ParentInterface extends InterfaceA, InterfaceB {}
interface ChildInterface extends ParentInterface {}

function main() {
    const obj: ChildInterface = {
        propA: "Hello",
        propB: 42
    }
    assertEQ(obj.propA, "Hello")
    assertEQ(obj.propB, 42)
}