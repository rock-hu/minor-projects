/**
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

export let arrObj = new Array<boolean>;

export let ARRNUM1: number = 0;
export let ARRNUM2: number = 10;
export let ARRNUM3: number = 999;

export let testArr1 = new Array<Number>(ARRNUM1, ARRNUM2, ARRNUM3);
export let testArr2 = new Array<Number>(1, 2, 3);

export class User {
    userName_: string;
    userIdx_: number;

    constructor(userName: string, userIdx: number) {
        this.userName_ = userName;
        this.userIdx_ = userIdx;
    }

    get userName(): string {
        return this.userName_;
    }
    set userName(newUsername: string) {
        this.userName_ = newUsername;
    }

    get userIdx(): number {
        return this.userIdx_;
    }
    set userIdx(newIdx: number) {
        this.userIdx_ = newIdx;
    }

    GetUserName(): string {
        return this.userName_;
    }
    GetUserIdx(): number {
        return this.userIdx_;
    }
}

export function InsertArray(num: boolean): void {
    arrObj.push(num);
}

export function GetArrLength(): number {
    return arrObj.length;
}

export function SetArrLength(len: number): void {
    arrObj.length = len;
}

export function IsEqual(): boolean {
    return testArr2[0] === ARRNUM1 && testArr2[1] === ARRNUM2 && testArr2[2] === ARRNUM3;
}
