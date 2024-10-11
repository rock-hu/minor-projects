/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



class Ability {
    propertyA: string = "";
    propertyB: string = "";
}

class GlobalThis {
    ability!: Ability;
}

class Time {
    static INFO: number[] = [];
    static START: number = 2000;
}

function test1_1(globalThis: GlobalThis) {
    logger.info(globalThis.ability.propertyA + globalThis.ability.propertyB);
}

function test1_2(year: number): number {
    let totalDays: number = 348;
    for (let index: number = 0x8000; index > 0x8; index >>= 1) {
        totalDays += ((Time.INFO[year - Time.START] & index) !== 0) ? 1 : 0;
    }

    return totalDays;
}