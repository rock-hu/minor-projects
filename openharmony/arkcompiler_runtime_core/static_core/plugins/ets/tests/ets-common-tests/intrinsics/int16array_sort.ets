/*
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

const ARRAY_SIZE = 15;
const AB_SIZE = ARRAY_SIZE * Short.BYTE_SIZE;
const UNSORTED = [5, 3, 127, 2, 4, -1, 99, 10, 32767, 7, 0, 45, 1, 88, -32768];
const SORTED = [-32768, -1, 0, 1, 2, 3, 4, 5, 7, 10, 45, 88, 99, 127, 32767];
const SORTED_UNALIGNED = [-32768, -1, 0, 1, 2, 3, 4, 7, 10, 45, 88, 99, 127, 32767];
const SINGLE = [88];

function fillBuffer(buf: ArrayBuffer): void {
    assertEQ(Short.BYTE_SIZE, Int16Array.BYTES_PER_ELEMENT);
    assertEQ(Short.BYTE_SIZE, 2);
    for (let i = 0; i < AB_SIZE; ++i) {
        if ((i % Short.BYTE_SIZE) == 0) {
            let v: short = UNSORTED[i / 2] as short;
            if (Runtime.isLittleEndianPlatform()) {
                buf.set(i, v as byte);
                buf.set(i + 1, (v >>> 8) as byte);
            } else {
                buf.set(i, (v >>> 8) as byte);
                buf.set(i + 1, v as byte);
            }
        }
    }
}

function checkSorted(a: Int16Array): void {
    for (let i = 0; i < ARRAY_SIZE; ++i) {
        assertEQ(a[i], SORTED[i]);
    }
}

function checkSortedUnaligned(a: Int16Array): void {
    for (let i = 0; i < ARRAY_SIZE - 1; ++i) {
        assertEQ(a[i], SORTED_UNALIGNED[i]);
    }
}

function sortEmpty(): void {
    let empty: Int16Array = new Int16Array();
    empty.sort();
}

function sortSingle(): void {
    let single: Int16Array = new Int16Array(SINGLE);
    single.sort();
    assertEQ(single[0], SINGLE[0]);
}

function sortNormal(): void {
    let a: Int16Array = new Int16Array(UNSORTED);
    a.sort();
    checkSorted(a);
}

function sortUnaligned(): void {
    // Introduce 2-byte offset to test the case when underlying array data
    // does not start at 64-bit aligned address
    let ab: ArrayBuffer = new ArrayBuffer(AB_SIZE);
    let aUnaligned: Int16Array = new Int16Array(ab, 2 /* 2-byte offset */);
    fillBuffer(ab);
    aUnaligned.sort();
    checkSortedUnaligned(aUnaligned);
}

function main(): int {
    assertEQ(UNSORTED.length, SORTED.length);
    const suite = new ArkTestsuite('Int16Array.sort() tests')
    suite.addTest('sort an empty Int16Array', sortEmpty)
    suite.addTest('sort a single-element Int16Array', sortSingle)
    suite.addTest('sort a normal Int16Array', sortNormal)
    suite.addTest('sort a normal Int16Array backed with unaligned ArrayBuffer', sortUnaligned)
    return suite.run()
}
