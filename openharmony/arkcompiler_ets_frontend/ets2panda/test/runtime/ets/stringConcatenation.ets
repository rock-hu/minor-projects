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

function main() {
    let src: string | undefined = undefined;
    let s = "_str_";
    let udef = undefined;
    let a :string = "A_";
    let n : string | undefined | null = null;

    let dst: string = "" + src;
    let dst_s : string = src + "";
    let a_dst :string = a + dst;
    let n_dst : string = n + a_dst;
    let dst_a : string = dst + a;
    
    

    assertEQ(dst, "undefined")
    assertEQ(dst_s, "undefined")
    assertEQ(a_dst, "A_undefined")
    assertEQ(dst_a, "undefinedA_")
    assertEQ(n_dst, "nullA_undefined")
    assertEQ(udef + s, "undefined_str_")
    assertEQ(s + udef, "_str_undefined")
    assertEQ(a + n, "A_null")
    assertEQ(a + s, "A__str_")
    assertEQ(a + s + udef, "A__str_undefined")
    assertEQ(a + udef + a, "A_undefinedA_")
    assertEQ(udef + s + n, "undefined_str_null")

    let p1 = 1
    let p2 = 2
    let p3 = "*"

    let res = p1+p2+p3;
    assertEQ(res, "3*")
}
