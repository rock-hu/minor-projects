/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

declare function print(str:any):string;
declare var ArkTools:any;
import { DataManager } from "./ds_manager"
function contextScope() {
    let a: number = 1
    function context() {
        let b: number = a + 1
        DataManager.getInstance().insertEvent()
    }
    context()
}

contextScope()
ArkTools.excutePendingJob()
print("========================")
DataManager.getInstance().insertEvent()