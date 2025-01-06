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

import { assert, describe, expect, it } from 'vitest';
import { Scene } from '../../../../src/Scene';
import { SceneConfig } from '../../../../src/Config';
import path from 'path';
import { Builtin } from '../../../../src/core/common/Builtin';
import { Local, Stmt, Value } from '../../../../src';

let config: SceneConfig = new SceneConfig();
config.buildFromProjectDir(path.join(__dirname, "../../../resources/model/method"))
let projectScene: Scene = new Scene();
projectScene.buildSceneFromProjectDir(config);
projectScene.inferTypes();

let arkFile = projectScene.getFiles().find((file) => file.getName() == 'method.ts');
let arkDefaultClass = arkFile?.getDefaultClass();
let nestedTestClass = arkFile?.getClassWithName('NestedTestClass');

describe("ArkMethod Test", () => {
    it('test dotDotDot parameter', async () => {
        let method = arkDefaultClass?.getMethodWithName('testDotDotDotToken');
        let parameter = method?.getParameters().find((param) => param.getName() == 'arr2');
        if (parameter) {
            const hasDotDotDotToken = parameter.hasDotDotDotToken();
            expect(hasDotDotDotToken).eq(true);
        }
    })

    it('test object parameter', async () => {
        let method = arkDefaultClass?.getMethodWithName('testObjectTypeParam');
        let parameter = method?.getParameters().find((param) => param.getName() == 'obj');
        if (parameter) {
            const paramTypeName = parameter.getType().toString();
            expect(paramTypeName).eq(Builtin.OBJECT_CLASS_SIGNATURE.toString());
        }
    })
});

describe('Nested Method with Function Declaration Statement', () => {
    it('test nested method', async () => {
       const method = arkDefaultClass?.getMethodWithName('%innerFunction1$outerFunction1');
       assert.isDefined(method);
       assert.isNotNull(method);

       const stmts = method?.getBody()?.getCfg().getStmts();
       assert.isDefined(stmts);
       expect((stmts as Stmt[])[1].toString()).toEqual('instanceinvoke console.<@%unk/%unk: .log()>(\'This is nested function with function declaration.\')');
       expect((stmts as Stmt[])[2].toString()).toEqual('staticinvoke <@%unk/%unk: .innerInnerFunction1()>()');

       const global = method!.getBody()?.getUsedGlobals()?.get('innerInnerFunction1');
       assert.isDefined(global);

       // TODO： the type should be FunctionType after inferType support
       expect(global!.getType().toString()).toEqual('unknown');
    });

    it('test nested method in nested method', async () => {
        const method = arkDefaultClass?.getMethodWithName('%innerInnerFunction1$%innerFunction1$outerFunction1');
        assert.isDefined(method);
        assert.isNotNull(method);
        const stmts = method?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts);
        expect((stmts as Stmt[])[1].toString()).toEqual(
            'instanceinvoke console.<@%unk/%unk: .log()>(\'This is nested function in nested function with function declaration.\')');
        const locals = method?.getBody()?.getLocals();
        assert.isDefined(locals);
        expect((locals as Map<string, Local>).size).toEqual(2);
    });

    it('test outer method', async() => {
        const method = arkDefaultClass?.getMethodWithName('outerFunction1');
        assert.isDefined(method);
        assert.isNotNull(method);

        const stmts = method?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts);
        expect((stmts as Stmt[])[1].toString()).toEqual('staticinvoke <@%unk/%unk: .innerFunction1()>()');

        const global = method!.getBody()?.getUsedGlobals()?.get('innerFunction1');
        assert.isDefined(global);

        // TODO： the type should be FunctionType after inferType support
        expect(global!.getType().toString()).toEqual('unknown');
    });
});

describe('Nested Method with Return Statement', () => {
    it('test nested method', async () => {
        const method = arkDefaultClass?.getMethodWithName('%innerFunction2$outerFunction2');
        assert.isDefined(method);
        assert.isNotNull(method);
        const stmts = method?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts);
        expect((stmts as Stmt[])[2].toString()).toEqual('%0 = \'This is nested function with return statement. param \' + param');
        const local = method?.getBody()?.getLocals().get('param');
        assert.isDefined(local);
        expect((local as Local).getType().toString()).toEqual('string');
    });

    it('test outer method', async() => {
        const method = arkDefaultClass?.getMethodWithName('outerFunction2');
        assert.isDefined(method);
        assert.isNotNull(method);
        const stmts = method?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts);
        expect((stmts as Stmt[])[2].toString()).toEqual('return %innerFunction2$outerFunction2');
        const functionLocal = method?.getBody()?.getLocals().get('%innerFunction2$outerFunction2');
        assert.isDefined(functionLocal);
        expect((functionLocal as Local).getType().toString()).toEqual('@method/method.ts: %dflt.%innerFunction2$outerFunction2(string)');
        const numberLocal = method?.getBody()?.getLocals().get('innerFunction2');
        assert.isDefined(numberLocal);
        expect((numberLocal as Local).getType().toString()).toEqual('number');
    });
});

describe('Nested Method with Function Expression', () => {
    it('test nested method', async () => {
        const method = arkDefaultClass?.getMethodWithName('%AM1$outerFunction3');
        assert.isDefined(method);
        assert.isNotNull(method);
        const stmts = method?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts);
        expect((stmts as Stmt[])[1].toString()).toEqual('instanceinvoke console.<@%unk/%unk: .log()>(\'This is nested function with function expression.\')');
        const locals = method?.getBody()?.getLocals();
        assert.isDefined(locals);
        expect(locals!.size).toEqual(1);
    });

    it('test outer method', async() => {
        const method = arkDefaultClass?.getMethodWithName('outerFunction3');
        assert.isDefined(method);
        assert.isNotNull(method);
        const stmts = method?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts);
        expect((stmts as Stmt[])[1].toString()).toEqual('innerFunction3 = %AM1$outerFunction3');
        const leftOpLocal = method?.getBody()?.getLocals().get('innerFunction3');
        assert.isDefined(leftOpLocal);
        expect((leftOpLocal as Local).getType().toString()).toEqual('@method/method.ts: %dflt.%AM1$outerFunction3()');
        assert.isTrue((leftOpLocal as Local).getConstFlag());
        const rightOpLocal = method?.getBody()?.getLocals().get('%AM1$outerFunction3');
        assert.isDefined(rightOpLocal);
        expect((rightOpLocal as Local).getType().toString()).toEqual('@method/method.ts: %dflt.%AM1$outerFunction3()');
        expect((stmts as Stmt[])[2].toString()).toEqual('ptrinvoke <@%unk/%unk: .innerFunction3()>()');
    });
});

describe('Nested Method with Arrow Function', () => {
    it('test nested method', async () => {
        const method = arkDefaultClass?.getMethodWithName('%AM2$outerFunction4');
        assert.isDefined(method);
        assert.isNotNull(method);
        const stmts = method?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts);
        expect((stmts as Stmt[])[1].toString()).toEqual('instanceinvoke console.<@%unk/%unk: .log()>(\'This is nested function with arrow function.\')');
        const locals = method?.getBody()?.getLocals();
        assert.isDefined(locals);
        expect(locals!.size).toEqual(1);
    });

    it('test outer method', async() => {
        const method = arkDefaultClass?.getMethodWithName('outerFunction4');
        assert.isDefined(method);
        assert.isNotNull(method);
        const stmts = method?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts);
        expect((stmts as Stmt[])[1].toString()).toEqual('innerFunction4 = %AM2$outerFunction4');
        const leftOpLocal = method?.getBody()?.getLocals().get('innerFunction4');
        assert.isDefined(leftOpLocal);
        expect((leftOpLocal as Local).getType().toString()).toEqual('@method/method.ts: %dflt.%AM2$outerFunction4()');
        assert.isTrue((leftOpLocal as Local).getConstFlag());
        const rightOpLocal = method?.getBody()?.getLocals().get('%AM2$outerFunction4');
        assert.isDefined(rightOpLocal);
        expect((rightOpLocal as Local).getType().toString()).toEqual('@method/method.ts: %dflt.%AM2$outerFunction4()');
        expect((stmts as Stmt[])[2].toString()).toEqual('ptrinvoke <@%unk/%unk: .innerFunction4()>()');
    });
})

describe('Recursive Method', () => {
    it('test nested method', async () => {
        const method = arkDefaultClass?.getMethodWithName('%factorial$outerFunction5');
        assert.isDefined(method);
        assert.isNotNull(method);

        const global = method!.getBody()?.getUsedGlobals()?.get('factorial');
        assert.isDefined(global);

        // TODO： the type should be FunctionType after inferType support
        expect(global!.getType().toString()).toEqual('unknown');

        expect(method!.getReturnType().toString()).toEqual('number');
    });

    it('test outer method', async() => {
        const method = arkDefaultClass?.getMethodWithName('outerFunction5');
        assert.isDefined(method);
        assert.isNotNull(method);

        const global = method!.getBody()?.getUsedGlobals()?.get('factorial');
        assert.isDefined(global);

        // TODO： the type should be FunctionType after inferType support
        expect(global!.getType().toString()).toEqual('unknown');

        expect(method!.getReturnType().toString()).toEqual('number');
    })
})

describe('Nested Method in Class', () => {
    it('test nested method', async () => {
        const method1 = nestedTestClass?.getMethodWithName('%innerFunction1$outerMethod');
        assert.isDefined(method1);
        assert.isNotNull(method1);
        const stmts1 = method1?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts1);
        expect((stmts1 as Stmt[])[1].toString()).toEqual('instanceinvoke console.<@%unk/%unk: .log()>(\'innerFunction1\')');

        const method2 = nestedTestClass?.getMethodWithName('%AM1$outerMethod');
        assert.isDefined(method2);
        assert.isNotNull(method2);
        const stmts2 = method2?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts2);
        expect((stmts2 as Stmt[])[1].toString()).toEqual('instanceinvoke console.<@%unk/%unk: .log()>(\'innerFunction2\')');

        const method3 = nestedTestClass?.getMethodWithName('%AM2$outerMethod');
        assert.isDefined(method3);
        assert.isNotNull(method3);
        const stmts3 = method3?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts3);
        expect((stmts3 as Stmt[])[1].toString()).toEqual('instanceinvoke console.<@%unk/%unk: .log()>(\'innerFunction3\')');

        const method4 = nestedTestClass?.getMethodWithName('%AM3$outerMethod');
        assert.isDefined(method4);
        assert.isNotNull(method4);
        const stmts4 = method4?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts4);
        expect((stmts4 as Stmt[])[1].toString()).toEqual('instanceinvoke console.<@%unk/%unk: .log()>(\'innerFunction4\')');
    });

    it('test outer method', async () => {
        const outerMethod = nestedTestClass?.getMethodWithName('outerMethod');
        assert.isDefined(outerMethod);
        assert.isNotNull(outerMethod);

        const stmts = outerMethod?.getBody()?.getCfg().getStmts();
        assert.isDefined(stmts);
        expect((stmts as Stmt[])[1].toString()).toEqual('staticinvoke <@%unk/%unk: .innerFunction1()>()');
        expect((stmts as Stmt[])[4].toString()).toEqual('ptrinvoke <@%unk/%unk: .innerFunction2()>()');
        expect((stmts as Stmt[])[5].toString()).toEqual('ptrinvoke <@%unk/%unk: .innerFunction3()>()');

        const locals = outerMethod?.getBody()?.getLocals();
        assert.isDefined(locals);
        let innerFunction1Value: Value | undefined = locals?.get('innerFunction1');
        assert.isUndefined(innerFunction1Value);

        const globals = outerMethod?.getBody()?.getUsedGlobals();
        assert.isDefined(globals);
        innerFunction1Value = globals!.get('innerFunction1');
        assert.isDefined(innerFunction1Value);

        // TODO： the type should be FunctionType after inferType support
        expect(innerFunction1Value!.getType().toString()).toEqual('unknown');
        const innerFunction2Local = locals?.get('innerFunction2');
        assert.isDefined(innerFunction2Local);
        expect(innerFunction2Local!.getType().toString()).toEqual('@method/method.ts: NestedTestClass.%AM1$outerMethod()');
        const innerFunction3Local = locals?.get('innerFunction3');
        assert.isDefined(innerFunction3Local);
        expect(innerFunction3Local!.getType().toString()).toEqual('@method/method.ts: NestedTestClass.%AM2$outerMethod()');
    });
})
