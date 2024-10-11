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
 * A simple utility class.
 */
declare class Utility {
  /**
   * Constructor for Utility.
   */
  constructor();

  /**
   * Concatenates two strings.
   * @param {string} str1 - The first string.
   * @param {string} str2 - The second string.
   * @returns {string} The concatenated string.
   */
  concatenateStrings(str1: string, str2: string): string;

  /**
   * A method to check if a number is even.
   * @param {number} num - The number to check.
   * @returns {boolean} True if the number is even, false otherwise.
   */
  isEven(num: number): boolean;
}

/**
 * An interface representing a user profile.
 * @interface
 */
declare interface UserProfile {
  username: string;
  age: number;
}

/**
 * A function to greet a user.
 * @param {UserProfile} user - The user profile.
 * @returns {string} A personalized greeting.
 */
declare function greetUser(user: UserProfile): string;
