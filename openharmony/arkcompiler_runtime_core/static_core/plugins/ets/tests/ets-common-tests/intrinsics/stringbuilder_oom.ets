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
function foo(n: int): int { return (n * 17 + 7) % 141;}
function main(): int {
    let c: FixedArray<char> = new char[10000];
    let sb: StringBuilder = new StringBuilder();
    for (let i = 0; i < 10000000; ++i) {
        let n: int = foo(i);
        try {
            let s: String = new String(c);
            sb.append(s);
            n += 100;
        } catch (e: OutOfMemoryError) {
            if (n != foo(i)) {
                return 3;
            }
            // Expected exception caught
            return 0;
        } catch (e) {
            // Unexpected exception caught
            return 1;
        }
    }
    return 2;
}
