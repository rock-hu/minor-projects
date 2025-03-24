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
import { Variable } from "./Variable";
import { BasicBlock } from "@ArkAnalyzer/src";
import { ScopeType } from "./ScopeType";

export class Scope {
    //null
    parentScope: Scope | null;
    childScopeList: Array<Scope>;
    defList: Array<Variable>;
    block: Set<BasicBlock>;
    //0
    scopeLevel: number;
    //UNKNOWN_TYPE
    scopeType: ScopeType;

    constructor(parent: Scope | null, defList: Array<Variable>, leve: number, type: ScopeType = ScopeType.UNKNOWN_TYPE) {
        this.parentScope = parent;
        this.childScopeList = new Array<Scope>();
        this.defList = defList;
        this.block = new Set<BasicBlock>();
        this.scopeLevel = leve;
        this.scopeType = type;
    }
    public setChildScope(child: Scope) {
        this.childScopeList.push(child);
    }

    addVariable(variable: Variable) {
        this.defList.push(variable);
    }
}
