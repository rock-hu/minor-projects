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

export class unary_operations  {
    private a: int = 3;
    private pref1: int = +this.a;
    private pref2: int = -this.a;
    private pref3: int = ++this.a;
    private pref4: int = --this.a;
    private pref5: int = ~this.a;
    private b: boolean = false;
    private pref6: boolean = !this.b;
    private post1: int = this.a++;
    private post2: int = this.a--;
}
