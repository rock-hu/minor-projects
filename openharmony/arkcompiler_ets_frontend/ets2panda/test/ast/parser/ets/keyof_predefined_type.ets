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

function main():void{
    let c1:keyof byte = /* @@ label1 */"field1"
    let c2:keyof Int = /* @@ label2 */"field1"
}

/* @@@ label1 Error TypeError: Type '"field1"' cannot be assigned to type '"toDouble"|"$_hashCode"|"add"|"toByte"|"unboxed"|"isGreaterThan"|"compareTo"|"toLong"|"createFromJSONValue"|"isLessEqualThan"|"isLessThan"|"toFloat"|"toString"|"mul"|"sub"|"isGreaterEqualThan"|"equals"|"div"|"toInt"|"toShort"|"toChar"|"valueOf"|"MIN_VALUE"|"MAX_VALUE"|"BIT_SIZE"|"BYTE_SIZE"'  */
/* @@@ label2 Error TypeError: Type '"field1"' cannot be assigned to type '"toDouble"|"$_hashCode"|"add"|"toByte"|"unboxed"|"isGreaterThan"|"compareTo"|"toLong"|"createFromJSONValue"|"isLessEqualThan"|"isLessThan"|"toFloat"|"toString"|"mul"|"sub"|"isGreaterEqualThan"|"equals"|"div"|"toInt"|"toShort"|"toChar"|"valueOf"|"MIN_VALUE"|"MAX_VALUE"|"BIT_SIZE"|"BYTE_SIZE"'  */
