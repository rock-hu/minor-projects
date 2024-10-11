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

/**
 * A custom error class for a specific application error.
 * @class
 * @extends {Error}
 */
declare class MyAppError extends Error {
  /**
  * Constructor for MyAppError.
  * @param {string} message - The error message.
  * @param {string} errorCode - The error code.
  */
  constructor(message: string, errorCode: string);
}

/**
 * A conditional type based on the type of a value.
 * @typedef {T extends string ? "StringType" : "NonStringType"} StringTypeChecker<T>
 */

/**
 * An assertion function to assert that a value is a truthy string.
 * @param {any} value - The value to assert.
 * @returns {asserts value is string} - The asserted truthy string.
 */
declare function assertTruthyString(value: any): asserts value is string;
