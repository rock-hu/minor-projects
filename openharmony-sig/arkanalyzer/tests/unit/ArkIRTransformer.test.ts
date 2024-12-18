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
import { ArkMethod, Scene, SceneConfig, Stmt } from '../../src';
import path from 'path';
import {
    BinaryExpression_Expect_IR, CallExpression_Expect_IR,
    LiteralExpression_Expect_IR,
    NewExpression_Expect_IR,
    Operator_Expect_IR,
    UnaryExpression_Expect_IR,
} from '../resources/arkIRTransformer/expression/ExpressionExpectIR';
import {
    CompoundAssignment_Expect_IR,
    Declaration_Expect_IR,
} from '../resources/arkIRTransformer/assignment/AssignmentExpectIR';
import { ModelUtils } from '../../src/core/common/ModelUtils';
import { DEFAULT_ARK_METHOD_NAME } from '../../src/core/common/Const';
import {
    ArrowFunction_Expect_IR,
    NoOverloadMethod_Expect_IR,
    NoOverloadMethodWithBody2_Expect_IR,
    NoOverloadMethodWithBody_Expect_IR,
    OverloadClassMethod_Expect_IR,
    OverloadInterfaceMethod_Expect_IR,
    OverloadMethod_Expect_IR,
    OverloadNamespaceMethod_Expect_IR,
} from '../resources/arkIRTransformer/function/FunctionExpectIR';

const BASE_DIR = path.join(__dirname, '../../tests/resources/arkIRTransformer');

function testMethodStmts(scene: Scene, filePath: string, expectStmts: any[]): void {
    const arkFile = scene.getFiles().find((file) => file.getName().endsWith(filePath));
    const arkMethod = arkFile?.getDefaultClass().getMethods()
        .find((method) => (method.getName() === DEFAULT_ARK_METHOD_NAME));
    const stmts = arkMethod?.getCfg()?.getStmts();
    if (!stmts) {
        assert.isDefined(stmts);
        return;
    }
    assertStmtsEqual(stmts, expectStmts);
}

function testFileStmts(scene: Scene, filePath: string, expectFileStmts: any): void {
    const arkFile = scene.getFiles().find((file) => file.getName().endsWith(filePath));
    if (!arkFile) {
        assert.isDefined(arkFile);
        return;
    }
    const methods = ModelUtils.getAllMethodsInFile(arkFile);
    for (const expectMethod of expectFileStmts.methods) {
        const expectMethodName = expectMethod.name;
        const method = methods.find((method) => method.getName() === expectMethodName);
        if (!method) {
            assert.isDefined(method);
            continue;
        }
        const stmts = method.getCfg()?.getStmts();
        if (!stmts) {
            assert.isDefined(stmts);
            continue;
        }
        assertStmtsEqual(stmts, expectMethod.stmts);
    }
}

function assertStmtsEqual(stmts: Stmt[], expectStmts: any[]): void {
    expect(stmts.length).toEqual(expectStmts.length);
    for (let i = 0; i < stmts.length; i++) {
        expect(stmts[i].toString()).toEqual(expectStmts[i].text);

        const operandOriginalPositions: any[] = [];
        for (const operand of stmts[i].getDefAndUses()) {
            const operandOriginalPosition = stmts[i].getOperandOriginalPosition(operand);
            if (operandOriginalPosition) {
                operandOriginalPositions.push(
                    [operandOriginalPosition.getFirstLine(), operandOriginalPosition.getFirstCol(),
                        operandOriginalPosition.getLastLine(), operandOriginalPosition.getLastCol()]);
            } else {
                operandOriginalPositions.push(operandOriginalPosition);
            }
        }
        expect(operandOriginalPositions).toEqual(expectStmts[i].operandOriginalPositions);
    }
}

function testMethodOverload(scene: Scene, filePath: string, methodName: string, expectMethod: any): void {
    const arkFile = scene.getFiles().find((file) => file.getName().endsWith(filePath));
    const arkMethod = arkFile?.getDefaultClass().getMethods().find((method) => (method.getName() === methodName));
    if (arkMethod === undefined) {
        assert.isDefined(arkMethod);
        return;
    }
    assertMethodLineEqual(arkMethod, expectMethod);
    assertMethodBodyEqual(arkMethod, true, expectMethod);
    assertMethodSignatureEqual(arkMethod, expectMethod);
}

function testClassMethodOverload(scene: Scene, filePath: string, className: string, methodName: string, expectMethod: any): void {
    const arkFile = scene.getFiles().find((file) => file.getName().endsWith(filePath));
    const arkClass = arkFile?.getClasses().find((cls) => cls.getName() === className);
    const arkMethod = arkClass?.getMethods().find((method) => (method.getName() === methodName));
    if (arkMethod === undefined) {
        assert.isDefined(arkMethod);
        return;
    }
    assertMethodLineEqual(arkMethod, expectMethod);
    assertMethodBodyEqual(arkMethod, true, expectMethod);
    assertMethodSignatureEqual(arkMethod, expectMethod);
}

function testNamespaceMethodOverload(scene: Scene, filePath: string, namespaceName: string, methodName: string, expectMethod: any): void {
    const arkFile = scene.getFiles().find((file) => file.getName().endsWith(filePath));
    const arkNamespace = arkFile?.getNamespaces().find((ns) => ns.getName() === namespaceName);
    const arkMethod = arkNamespace?.getDefaultClass().getMethods().find((method) => (method.getName() === methodName));
    if (arkMethod === undefined) {
        assert.isDefined(arkMethod);
        return;
    }
    assertMethodLineEqual(arkMethod, expectMethod);
    assertMethodBodyEqual(arkMethod, false);
    assertMethodSignatureEqual(arkMethod, expectMethod);
}

function testInterfaceMethodOverload(scene: Scene, filePath: string, interfaceName: string, methodName: string, expectMethod: any): void {
    const arkFile = scene.getFiles().find((file) => file.getName().endsWith(filePath));
    const arkClass = arkFile?.getClasses().find((cls) => cls.getName() === interfaceName);
    const arkMethod = arkClass?.getMethods().find((method) => (method.getName() === methodName));
    if (arkMethod === undefined) {
        assert.isDefined(arkMethod);
        return;
    }
    assertMethodLineEqual(arkMethod, expectMethod);
    assertMethodBodyEqual(arkMethod, false, expectMethod);
    assertMethodSignatureEqual(arkMethod, expectMethod);
}

function testNoMethodOverload(scene: Scene, filePath: string, methodName: string, expectMethod: any): void {
    const arkFile = scene.getFiles().find((file) => file.getName().endsWith(filePath));
    const arkMethod = arkFile?.getDefaultClass().getMethods().find((method) => (method.getName() === methodName));
    if (arkMethod === undefined) {
        assert.isDefined(arkMethod);
        return;
    }
    assertMethodLineEqual(arkMethod, expectMethod);
    assertMethodBodyEqual(arkMethod, false);
    assertMethodSignatureEqual(arkMethod, expectMethod);
}

function testNoMethodOverloadWithBody(scene: Scene, filePath: string, methodName: string, expectMethod: any): void {
    const arkFile = scene.getFiles().find((file) => file.getName().endsWith(filePath));
    const arkMethod = arkFile?.getDefaultClass().getMethods().find((method) => (method.getName() === methodName));
    if (arkMethod === undefined) {
        assert.isDefined(arkMethod);
        return;
    }
    assertMethodLineEqual(arkMethod, expectMethod);
    assertMethodBodyEqual(arkMethod, true, expectMethod);
    assertMethodSignatureEqual(arkMethod, expectMethod);
}

function assertMethodLineEqual(method: ArkMethod, expectMethod: any): void {
    const declareLines = method.getDeclareLines();
    const expectDeclareLines = expectMethod.methodDeclareLines;
    if (expectDeclareLines !== undefined) {
        if (expectDeclareLines === null) {
            assert.isNull(declareLines);
        } else {
            assert.isNotNull(declareLines);
            expect(declareLines?.length).toEqual(expectDeclareLines.length);
            declareLines?.forEach((line, index) => {
                expect(line).toEqual(expectDeclareLines[index]);
            });
        }
    }

    if (expectMethod.line !== undefined) {
        if (expectMethod.line === null) {
            assert.isNull(method.getLine());
        } else {
            expect(method.getLine()).toEqual(expectMethod.line);
        }
    }
}

function assertMethodBodyEqual(method: ArkMethod, expectBodyDefined: boolean, expectMethod?: any): void {
    const body = method.getBody();
    if (!expectBodyDefined) {
        assert.isUndefined(body);
        return;
    }
    expect(body?.getLocals().get('x')?.getName()).toEqual(expectMethod.body.locals.x.name);
}

function assertMethodSignatureEqual(method: ArkMethod, expectMethod?: any): void {
    const declareSignatures = method.getDeclareSignatures();
    const expectDeclareSignatures = expectMethod.methodDeclareSignatures;
    if (expectDeclareSignatures !== undefined) {
        if (expectDeclareSignatures === null) {
            assert.isNull(declareSignatures);
        } else {
            assert.isNotNull(declareSignatures);
            expect(declareSignatures?.length).toEqual(expectDeclareSignatures.length);
            declareSignatures?.find((signature, index) => {
                expect(signature.toString()).toEqual(expectDeclareSignatures[index].toString);
                expect(signature.getMethodSubSignature().getReturnType().toString()).toEqual(expectDeclareSignatures[index].methodSubSignature.returnType);
            });
        }
    }

    const implementationSignature = method.getImplementationSignature();
    const expectImplSignature = expectMethod.methodSignature;
    if (expectImplSignature !== undefined) {
        if (expectImplSignature === null) {
            assert.isNull(implementationSignature);
        } else {
            assert.isNotNull(implementationSignature);
            expect(implementationSignature?.toString()).toEqual(expectImplSignature.toString);
            expect(implementationSignature?.getMethodSubSignature().getReturnType().toString()).toEqual(expectImplSignature.methodSubSignature.returnType);
        }
    }
}

function buildScene(folderName: string) {
    let config: SceneConfig = new SceneConfig();
    config.buildFromProjectDir(path.join(BASE_DIR, folderName));
    let scene = new Scene();
    scene.buildSceneFromProjectDir(config);
    scene.inferTypes();
    return scene;
}

describe('expression Test', () => {
    const scene = buildScene('expression');

    it('test binary expression', async () => {
        testMethodStmts(scene, 'BinaryExpressionTest.ts', BinaryExpression_Expect_IR.stmts);
    });

    it('test unary expression', async () => {
        testMethodStmts(scene, 'UnaryExpressionTest.ts', UnaryExpression_Expect_IR.stmts);
    });

    it('test new expression', async () => {
        testMethodStmts(scene, 'NewExpressionTest.ts', NewExpression_Expect_IR.stmts);
    });

    it('test literal expression', async () => {
        testMethodStmts(scene, 'LiteralExpressionTest.ts', LiteralExpression_Expect_IR.stmts);
    });

    it('test operator', async () => {
        testMethodStmts(scene, 'OperatorTest.ts', Operator_Expect_IR.stmts);
    });

    it('test call expression', async () => {
        testMethodStmts(scene, 'CallExpressionTest.ts', CallExpression_Expect_IR.stmts);
    });
});

describe('assignment Test', () => {
    const scene = buildScene('assignment');

    it('test declaration', async () => {
        testMethodStmts(scene, 'DeclarationTest.ts', Declaration_Expect_IR.stmts);
    });

    it('test compound assignment', async () => {
        testMethodStmts(scene, 'CompoundAssignmentTest.ts', CompoundAssignment_Expect_IR.stmts);
    });
});

describe('function Test', () => {
    const scene = buildScene('function');

    it('test arrow function', async () => {
        testFileStmts(scene, 'ArrowFunctionTest.ts', ArrowFunction_Expect_IR);
    });

    it('test overload function', async () => {
        testMethodOverload(scene, 'OverloadFunctionTest.ts', 'overloadedFunction1', OverloadMethod_Expect_IR);
    });

    it('test overload class function', async () => {
        testClassMethodOverload(scene, 'OverloadFunctionTest.ts', 'OverloadClass', 'overloadedFunction2', OverloadClassMethod_Expect_IR);
    });

    it('test overload namespace function', async () => {
        testNamespaceMethodOverload(scene, 'OverloadFunctionTest.ts', 'overloadNamespace', 'overloadedFunction3', OverloadNamespaceMethod_Expect_IR);
    });

    it('test overload interface function', async () => {
        testInterfaceMethodOverload(scene, 'OverloadFunctionTest.ts', 'OverloadInterface', 'overloadedFunction4', OverloadInterfaceMethod_Expect_IR);
    });

    it('test no overload function without body', async () => {
        testNoMethodOverload(scene, 'OverloadFunctionTest.ts', 'function5', NoOverloadMethod_Expect_IR);
    });

    it('test no overload function with body', async () => {
        testNoMethodOverloadWithBody(scene, 'OverloadFunctionTest.ts', 'function6', NoOverloadMethodWithBody_Expect_IR);
    });

    it('test no overload function with body 2', async () => {
        testNoMethodOverloadWithBody(scene, 'OverloadFunctionTest.ts', 'function7', NoOverloadMethodWithBody2_Expect_IR);
    });
});