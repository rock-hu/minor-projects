/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// const a = 1
namespace GlobalThis {

    class word {

    }

    export function main() {
        let temp = globalThis
        temp;

        globalThis.FieldA = new word()

        // let wo = globalThis.FieldA
        // let wo2 = temp.FieldA
        // wo
        // wo2
        te()
    }

    function te() {
        // let globalThis = new word()
        let temp2 = globalThis
        let a = temp2.FieldA
    }
}

// GlobalThis.main()