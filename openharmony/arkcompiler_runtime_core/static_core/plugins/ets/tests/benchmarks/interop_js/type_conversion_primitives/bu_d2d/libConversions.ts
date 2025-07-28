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

export function binaryStringToNumberToString(bin: string): string {
    const res = parseInt(bin, 2);
    return String(res);
};

export function decimalStringToNumberToString(str: string): string {
    const res = Number(str);
    return String(res);
};

export function hexStringToNumberToString(str: string): string {
    const res = parseInt(str, 16);
    return String(res);
};

export function octalStringToNumberToString(str: string): string {
    const res = parseInt(str, 8);
    return String(res);
};

export function charToNumberToString(chr: string): string {
    const res = chr.charCodeAt(0);
    return String(res);
};
