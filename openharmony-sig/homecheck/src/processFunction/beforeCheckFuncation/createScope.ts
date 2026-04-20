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
import {
    ArkArrayRef,
    ArkAssignStmt,
    ArkIfStmt,
    ArkSwitchStmt,
    ArkThisRef,
    BasicBlock,
    DEFAULT_ARK_METHOD_NAME,
    Local,
    Scene,
    Stmt
} from "@ArkAnalyzer/src";
import { Scope } from "../../object/scope/Scope";
import { Variable } from "../../object/scope/Variable";
import { ScopeType } from "../../object/scope/ScopeType";
import { ScopeUtils, TempLocation } from "../../utils/Scopeutils";
import { VarInfo } from "../../object/scope/VarInfo";
import LoggerFactory from "../../utils/LoggerFactory";
import { CheckerStorage } from "../../object/CheckerStorage";

let firstBlock: BasicBlock | undefined;
let finishBlockSet: Set<BasicBlock>;
let isSwitchLastCase: boolean = false;
let gFinishIfStmtLines: number[] = [];

export function createScope(scene: Scene) {
    const logger = LoggerFactory.getLogger("createScope");
    const arkFiles = scene.getFiles();
    const scopeMap = new Map<string, Scope>();
    for (const file of arkFiles) {
        const firstScope = new Scope(null, new Array<Variable>(), 0);
        scopeMap.set(file.getFilePath(), firstScope);

        for (const oneClass of file.getClasses()) {
            for (const method of oneClass.getMethods()) {
                gFinishIfStmtLines = [];
                finishBlockSet = new Set();
                firstBlock = method.getBody()?.getCfg()?.getStartingBlock();
                if (firstBlock) {
                    let curScope = firstScope;
                    if (method.getName() !== DEFAULT_ARK_METHOD_NAME) {
                        curScope = new Scope(firstScope, new Array<Variable>(), 1);
                        firstScope.setChildScope(curScope)
                    }
                    blockProcess(firstBlock, curScope)
                } else {
                    logger.debug(`${oneClass.getName()}::${method.getName()} has no body.`)
                }
            }
        }
    }
    // 放入全局单例
    CheckerStorage.getInstance().setScopeMap(scopeMap);
}

function blockProcess(block: BasicBlock, parentScope: Scope) {
    let curScope = parentScope;
    const stmts = block.getStmts();
    if (stmts.length === 0) {
        return;
    } else if (isFirstThisBlock(block)) {
        const succBlocks = block.getSuccessors();
        if (succBlocks.length > 0) {
            return blockProcess(succBlocks[0], curScope);
        }
    }

    let isSwitchBlock = false;
    let nextScopeType = getScopeType(stmts[stmts.length - 1]);
    curScope.block.add(block);
    finishBlockSet.add(block);
    for (let i = 0; i < stmts.length; i++) {
        const stmt = stmts[i];
        if (i === stmts.length - 1) {
            if (isForStmtDefinedPart(stmts[stmts.length - 1], nextScopeType)) {
                curScope = genChildScope(curScope, ScopeType.FOR_CONDITION_TYPE);
                nextScopeType = ScopeType.UNKNOWN_TYPE;
            }
        }
        Object.defineProperty(stmt, "scope", { value: curScope });
        if (stmt instanceof ArkAssignStmt && !assignStmtProcess(stmt, curScope)) {
            continue;
        } else if (stmt instanceof ArkIfStmt) {
            gFinishIfStmtLines.push(stmt.getOriginPositionInfo().getLineNo());
        } else if (stmt instanceof ArkSwitchStmt) {
            isSwitchBlock = true;
        }
    }

    if (isSwitchBlock) {
        switchBlockProcess(block, curScope);
    } else {
        nextBlockPreprocess(block, curScope, nextScopeType);
    }
}

function isForStmtDefinedPart(stmt: Stmt, nextScopeType: ScopeType): boolean {
    if (stmt instanceof ArkAssignStmt &&
        nextScopeType === ScopeType.FOR_CONDITION_TYPE &&
        !gFinishIfStmtLines.includes(stmt.getOriginPositionInfo().getLineNo())) {
        return true;
    }
    return false;
}

/**
 * 获取语句的作用域类型
 *
 * @param stmt 语句对象
 * @returns 作用域类型
 */
export function getScopeType(stmt: Stmt): ScopeType {
    const text = stmt.getOriginalText();
    if (text) {
        if (text.startsWith('for (') || text.startsWith('for(')) {
            return ScopeType.CASE_TYPE;
        } else if (text.startsWith('while (') || text.startsWith('while(')) {
            return ScopeType.WHILE_TYPE;
        } else if (text.startsWith('if (') || text.startsWith('if(')) {
            return ScopeType.IF_TYPE;
        }
    }
    return ScopeType.UNKNOWN_TYPE;
}

function isFirstThisBlock(block: BasicBlock) {
    if (block.getPredecessors().length === 0) {
        const stmts = block.getStmts();
        if (stmts.length === 1) {
            if (stmts[0] instanceof ArkAssignStmt && stmts[0].getRightOp() instanceof ArkThisRef) {
                return true;
            }
        }
    }
    return false;
}

function genChildScope(curScope: Scope, scopeType: ScopeType) {
    let newScope = new Scope(curScope, new Array<Variable>(), curScope.scopeLevel + 1, scopeType);
    curScope.setChildScope(newScope);
    return newScope;
}

function assignStmtProcess(stmt: Stmt, curScope: Scope): boolean {
    const def = stmt.getDef();
    if (def instanceof Local) {
        if (def.getName() === "this") {
            return false;
        }
        const isForStmtThirdPart = gFinishIfStmtLines.includes(stmt.getOriginPositionInfo().getLineNo());
        if (ScopeUtils.whereIsTemp(stmt) === TempLocation.LEFT || (!isForStmtThirdPart && ScopeUtils.isDeclaringStmt(def.getName(), stmt))) {
            curScope.addVariable(new Variable(def.getName(), stmt));
        } else {
            getDefAndSetRedef(def.getName(), curScope, curScope, stmt, SetDefMode.REDEF);
        }
    } else if (def instanceof ArkArrayRef) {
        let base = def.getBase();
        if (base instanceof Local && !base.getName().includes('%')) {
            getDefAndSetRedef(base.getName(), curScope, curScope, stmt, SetDefMode.LEFTUSED);
        }
    }
    return true;
}

function getDefAndSetRedef(name: string, searchScope: Scope, varScope: Scope, stmt: Stmt, mode: SetDefMode): boolean {
    for (const variable of searchScope.defList ?? []) {
        if (variable.name !== name) {
            continue;
        }
        switch (mode) {
            case SetDefMode.REDEF:
                variable.redefInfo.add(new VarInfo(stmt, varScope));
                break;
            case SetDefMode.LEFTUSED:
                variable.leftUsedInfo.add(new VarInfo(stmt, varScope));
                break;
            default:
                break;
        }
        return true;
    }

    if (searchScope.parentScope !== null) {
        return getDefAndSetRedef(name, searchScope.parentScope, varScope, stmt, mode);
    }
    return false;
}

enum SetDefMode {
    REDEF = 0,
    LEFTUSED
}

function switchBlockProcess(block: BasicBlock, curScope: Scope) {
    const caseBlocks = block.getSuccessors();
    for (const caseBlock of caseBlocks) {
        finishBlockSet.add(caseBlock);
    }
    for (let i = 0; i < caseBlocks.length; i++) {
        if (i === caseBlocks.length - 1) {
            isSwitchLastCase = true;
        }
        blockProcess(caseBlocks[i], genChildScope(curScope, ScopeType.CASE_TYPE));
    }
    isSwitchLastCase = false;
}

function genReturnScope(succBlock: BasicBlock, curScope: Scope): Scope {
    const stmts = succBlock.getStmts();
    if (stmts.length === 0) {
        return curScope;
    }
    const type = ScopeUtils.getScopeType(stmts[0]);
    if ([ScopeType.WHILE_TYPE, ScopeType.FOR_CONDITION_TYPE].includes(type)) {
        return curScope;
    } else {
        const returnScopeLevel = curScope.scopeLevel - succBlock.getPredecessors().length + 1;
        let exitNum = curScope.scopeLevel - returnScopeLevel;
        while (exitNum) {
            if (curScope.parentScope !== null) {
                curScope = curScope.parentScope;
                exitNum--;
            } else {
                break;
            }
        }
        return curScope;
    }
}

function nextBlockPreprocess(block: BasicBlock, curScope: Scope, nextScopeType: ScopeType) {
    const succBlocks = block.getSuccessors();
    for (let i = 0; i < succBlocks.length; i++) {
        if (finishBlockSet.has(succBlocks[i])) {
            continue;
        } else if (i === 0) {
            if (isNeedCreateScope(nextScopeType)) {
                const type = nextScopeType === ScopeType.FOR_CONDITION_TYPE ? ScopeType.FOR_IN_TYPE : nextScopeType
                blockProcess(succBlocks[i], genChildScope(curScope, type))
            } else {
                if (isSwitchLastCase) {
                    isSwitchLastCase = false;
                    curScope = curScope.parentScope!;
                }
                blockProcess(succBlocks[i], genReturnScope(succBlocks[i], curScope))
            }
        } else {
            switch (nextScopeType) {
                case ScopeType.FOR_CONDITION_TYPE:
                    blockProcess(succBlocks[i], genReturnScope(succBlocks[i], curScope.parentScope!));
                    break;
                case ScopeType.WHILE_TYPE:
                    blockProcess(succBlocks[i], genReturnScope(succBlocks[i], curScope));
                    break;
                default:
                    blockProcess(succBlocks[i], genChildScope(curScope, ScopeType.ELSE_TYPE));
                    break;
            }
        }
    }
}

function isNeedCreateScope(nextScopeType: ScopeType) {
    if (nextScopeType === ScopeType.FOR_CONDITION_TYPE || nextScopeType === ScopeType.WHILE_TYPE || nextScopeType === ScopeType.IF_TYPE) {
        return true;
    }
    return false;
}
