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
import { ViewTreeNode } from "@ArkAnalyzer/src"
import { ArkClass } from "@ArkAnalyzer/src"

export default class ViewTreeTool {
    private recordMap: Map<string, boolean>

    constructor() {
        this.recordMap = new Map();
    }

    hasTraverse(item: ArkClass | ViewTreeNode): boolean {
        let classSig = '';
        if (item instanceof ArkClass) {
            classSig = item.getSignature().toString();
        } else {
            if (item.signature) {
                classSig = item.signature.toString();
            } else {
                return false;
            }
        }
        let result = this.recordMap.get(classSig);
        this.recordMap.set(classSig, true);
        return result === true;
    }
}