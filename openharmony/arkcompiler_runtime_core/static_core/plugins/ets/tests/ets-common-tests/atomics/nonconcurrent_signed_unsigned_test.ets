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

function main() {
    let sab = new ArrayBuffer(1024);
    let ab = new ArrayBuffer(256);

    let view6 = new Uint32Array(sab, 32, 20);
    let control6 = new Uint32Array(ab, 0, 2);

    view6[3] = -5;
    control6[0] = -5;

    arktest.assertEQ(Atomics.or(view6, 3, 0), control6[0]);
    
    let bsab = new ArrayBuffer(1024);
    let bab = new ArrayBuffer(256);

    let bview6 = new Uint32Array(bsab, 32, 20);
    let bcontrol6 = new Uint32Array(bab, 0, 2);

    bview6[3] = -5;
    bcontrol6[0] = -5;

    arktest.assertEQ(Atomics.or(bview6, 3, 0), bcontrol6[0]);
}
