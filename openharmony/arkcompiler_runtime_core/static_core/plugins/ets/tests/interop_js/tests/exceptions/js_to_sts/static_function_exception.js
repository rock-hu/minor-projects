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


//Static function throws builtin exception
export const staticFunctionWithBuiltinException = ()=>{
    throw new TypeError('This is a TypeError');
};


//Static function throws user-defined exception E
export class CustomError extends Error {
    constructor(message) {
        super(message);
        this.name = 'CustomErrorA';
    }
}

export const staticFunctionWithCustomException = ()=>{
    throw new CustomError('This is a custom error(from static function)');
};

export function createError(name, message) {
    const error = new Error(message);
    error.name = name;
    return error;
}
