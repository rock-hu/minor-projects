/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http: //www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

function foo1(p1: int, p2?: int, p3: int = 22) { }

function foo2(p1: int, p2: int =44 , p3: int = 55) { }

function foo3(p1: int, p2: int = 44, p3?: int) { }


function main(){
    foo1(1, 1, 1);
    foo2(1, 1, 1);
    foo3(1, 1, 1);

}