/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import {
    TsExport
} from './cpp_test_dynamic_js.js'

@interface MyAnnoName {
    a: number = 3
    b: number[] = [13, 9]
    d: boolean = false
    str: string = 'Hello'
}

export function bar() {
    print('bar logic')
}



@__$$ETS_ANNOTATION$$__MyAnnoName({
    a: 20,
    b: [13, 10],
    d: true,
    str: 'ABC'
})
class A {
    @__$$ETS_ANNOTATION$$__MyAnnoName({
        a: 10,
        b: [1, 2, 3],
        d: true,
        str: 'DEF'
    })
    foo() {
        print('foo logic')
        bar()
    }
}

namespace MyNamespace {
    class B {
        zet() {
            return new TsExport()
        }
    }
}

(new A()).foo()
