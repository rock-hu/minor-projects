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

class Residence {
    numberOfRooms: Int = 1;
}

class Person {
    residence: Residence = new Residence();
}

function dummy(): Person | null {
    return null;
}

function main(): void {
    let john: Person | null = dummy();
    let test = john?.residence?.numberOfRooms;
    assertTrue(test == null)
    let nullsafety_test = john?.residence?.numberOfRooms ?? "unknown";
    assertEQ(nullsafety_test, "unknown")
}
