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

interface AInterface<T> {}
interface BInterface<T> extends AInterface<T> {}
final class B<T> implements BInterface<T>{}

final class StaticClass<T> implements AInterface<T>{
    public static staticFunction<T extends Number>(arg: AInterface<T>) : StaticClass<T> {
        return new StaticClass<T>();
    }
}

function func (arg: B<String>) {
    return /* @@ label */StaticClass.staticFunction(arg);
}

function main() {
    let a = func(new B<String>());
}

/* @@@ label Error TypeError: No matching call signature for staticFunction(B<String>)  */
/* @@? 31:13 Error TypeError: Cannot use type 'void' as value.  */
