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

// keep all
class KeepProperty3 {
    "prop1": string = '1';
    "prop2": number = 2;
    "******": number = 3;
    "abcdef": number = 4;
    "a*": number = 5;
    "/\\*": number = 6;
    prop: string = '1';

    functionkeep() {
        return `function name should be kept.`;
    }
}