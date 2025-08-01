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

class Thenable implements PromiseLike<string> {
    constructor(e: Error) {
        this.error = e;
    }
    then<U, E>(_?: (value: string) => U | PromiseLike<U>, onRejected?:
            (error: Error) => E | PromiseLike<E>): PromiseLike<U | E> {
        return Promise.resolve().then<E>((): E | PromiseLike<E> => {
            return onRejected!(this.error);
        });
    }
    private error: Error;
};

let error = new Error('abc');
Promise.race([new Thenable(error)]).then<void, void>((values: string): void => {
    console.log('Test failed. The promise should not be resolved.');
}, (e: Error): void => {
    if (error !== e) {
        console.log('Test failed. The promise should be rejected by the specified value.');
        return;
    }
    console.log('Test passed.');
});
