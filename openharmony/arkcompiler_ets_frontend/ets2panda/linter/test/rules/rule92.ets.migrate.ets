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

// Fixable

function addNum(a: number, b: number): void {
    let logToConsole: (message: String) => void = (message: String): void => {
    console.log(message);
};

    let result = a + b

    logToConsole("result is " + result)
}


function addNum2(a: number, b: number): void {
    // Use lambda instead of a nested function:
    let logToConsole: (message: string) => void = (message: string): void => {
        console.log(message)
    }

    let result = a + b

    logToConsole("result is " + result)
}


function NestedOne(a: number, b: number): void {
    let NestedTwo: (message: String) => void = (message: String): void => {
    let NestedThree: (message: String) => void = (message: String): void => {
    console.log("NestedThree");
};
};
}


function NestedOne1(a: number, b: number): void {
    let NestedTwo: (message: String) => void = (message: String): void => {
    console.log("NestedTwo");
};

    let NestedTooToo: (message: String) => void = (message: String): void => {
    console.log("NestedTooToo");
};
}

function NestedOne2(a: number, b: number): void {
    let NestedTwo: (message: String) => void = (message: String): void => {
    let NestedThree: (message: String) => void = (message: String): void => {
    console.log("NestedThree");
};
    let NestedThreeToo: (message: String) => void = (message: String): void => {
    console.log("NestedThreeToo");
};
};
}

// Not fixable
function decoratorFoo(): void {
    @decorator
    function decorated(): void {
    }

    function* generatorFunction() {
        return 3;
    }

    function thisFoo() {
        return this.what;
    }

    notFixable();

    function notFixable(): void {
    }
}

