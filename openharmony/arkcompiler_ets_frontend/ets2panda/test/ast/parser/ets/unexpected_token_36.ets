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

export class AccessNSieve {
  static readonly n1: int  = 3;
  static readonly n2: int = 10000;
  static readonly expected: int = 14302;
  static isPrime: boolean[]

  public setup(): void {
    AccessNSieve.isPrime = new boolean[(1 << AccessNSieve.n1) * AccessNSieve.n2 + 1];
  }

  private static nsieve(m: int): int {
    let count: int = 0;

    for (le i: int = 2; i <= m; i++) {
      AccessNSieve.isPrime[i] = true;
    }
  }
}
/* @@? 29:10 Error TypeError: Unresolved reference le */
/* @@? 29:13 Error SyntaxError: Expected ';', got 'identification literal'. */
/* @@? 29:13 Error TypeError: Unresolved reference i */
/* @@? 29:14 Error SyntaxError: Expected ';', got ':'. */
/* @@? 29:14 Error SyntaxError: Unexpected token ':'. */
/* @@? 29:16 Error SyntaxError: Expected ')', got 'int'. */
/* @@? 29:16 Error SyntaxError: Unexpected token 'int'. */
/* @@? 29:25 Error TypeError: Bad operand type, the types of the operands must be numeric, same enumeration, or boolean type. */
/* @@? 29:36 Error SyntaxError: Unexpected token ')'. */
/* @@? 29:38 Error SyntaxError: Unexpected token '{'. */
