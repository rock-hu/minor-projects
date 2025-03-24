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
import { ArkAssignStmt, Local, Stmt } from "@ArkAnalyzer/src";
import { ScopeType } from "../object/scope/ScopeType";

export class ScopeUtils {

    public static whereIsTemp(stmt: Stmt) {
        let def = stmt.getDef();
        if (def instanceof Local) {
            if (def.getName().includes('%')) {
                return TempLocation.LEFT;
            }
        }
        if (stmt instanceof ArkAssignStmt) {
            let right = stmt.getRightOp();
            if (right instanceof Local && right.getName().includes('%')) {
                return TempLocation.RIGHT;
            }
        }
        return TempLocation.NOTFOUND
    }

    public static isDeclaringStmt(defName: string, stmt: Stmt) {
        const text = stmt.getOriginalText();
        if (text) {
            if (text.includes('let ' + defName) || text.includes('const ' + defName) || text.includes('var ' + defName)) {
                const c = text[text.indexOf(' ' + defName) + defName.length + 1];
                if (c === ' ' || c === ':' || c === '=') {
                    return true;
                }
            }
        }
        return false;
    }

    public static getScopeType(stmt: Stmt) {
        let text = stmt.getOriginalText();
        if (text !== undefined) {
            if (text.startsWith('for(') || text.startsWith('for(')) {
                return ScopeType.FOR_CONDITION_TYPE;
            } else if (text.startsWith('while(') || text.startsWith('while(')) {
                return ScopeType.WHILE_TYPE;
            } else if (text.startsWith('if(') || text.startsWith('if(')) {
                return ScopeType.IF_TYPE;
            }
        } else {

        }
        return ScopeType.UNKNOWN_TYPE;
    }
}
export enum TempLocation {
    NOTFOUND,
    LEFT,
    RIGHT,
}