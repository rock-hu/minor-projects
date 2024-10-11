/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

export let value01 = 1, value02 = 2;

export const value03 = 3, value04 = 4/*, … */; // also var, let

export function function01() { 
    return 10;
}

export class Actor { 
    name = 'asuss';
    age = 19;
}

export function* generatorFunctionName01() { 
    let acator = new Actor();
    acator.age += 1;
    yield acator;
}
