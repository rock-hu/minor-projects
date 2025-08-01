/*
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

function checkSequence(actual: Array<number>, expectedLength: number): void {
    if (actual.length != expectedLength) {
        console.log('Test failed. Expected sequence length ' + expectedLength + ', but got ' + actual.length);
        throw new Error('Test failed');
    }
    for (let i = 0; i < expectedLength; ++i) {
        if (actual[i] != i + 1) {
            console.log('Test failed. Expected element at ' + i + ' to be ' + i + ', but got ' + actual[i]);
            throw new Error('Test failed');
        }
    }
}

let sequence = new Array<number>();
let resolve: ((value: number) => void) | null = null;
let reject: ((err: Error) => void) | null = null;
new Promise<number>((_resolve: (v: number) => void, _: (error: Error) => void) => {
    resolve = _resolve;
}).then<void>((): void => {
    sequence.push(3)
}).then<void>((): void => {
    checkSequence(sequence, 3);
    console.log('Test passed.');
});
new Promise<number>((_: (v: number) => void, _reject: (error: Error) => void) => {
    reject = _reject;
}).catch<void>((): void => {
    sequence.push(2);
});

reject!(new Error('abc'));
resolve!(7);

sequence.push(1);
