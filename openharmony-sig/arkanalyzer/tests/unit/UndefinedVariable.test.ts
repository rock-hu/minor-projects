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

import { Scene, SceneConfig, UndefinedVariableChecker, UndefinedVariableSolver } from '../../src/index';
import { describe, expect, it } from 'vitest';


function buildScene(projectPath: string): Scene {
    let config: SceneConfig = new SceneConfig();
    config.buildFromProjectDir(projectPath);
    let scene = new Scene();
    scene.buildSceneFromProjectDir(config);
    scene.inferTypes();
    return scene;
}

const scene = buildScene('tests/resources/ifds/UndefinedVariable');

describe('UndefinedVariableTest1', () => {
    const u1 = scene.getMethods().filter(method => method.getName() === 'u1')[0];
    const problem = new UndefinedVariableChecker([...u1.getCfg()!.getBlocks()][0].getStmts()[u1.getParameters().length], u1);
    const solver = new UndefinedVariableSolver(problem, scene);
    solver.solve();
    it('case1: UndefinedVariable', () => {
        expect(problem.getOutcomes().length).eq(0);
    });
});

describe('UndefinedVariableTest2', () => {
    const u2 = scene.getMethods().filter(method => method.getName() === 'u2')[0];
    const problem = new UndefinedVariableChecker([...u2.getCfg()!.getBlocks()][0].getStmts()[u2.getParameters().length], u2);
    const solver = new UndefinedVariableSolver(problem, scene);
    solver.solve();
    it('case1: UndefinedVariable', () => {
        expect(problem.getOutcomes().length).eq(1);
    });
});

describe('UndefinedVariableTest3', () => {
    const u3 = scene.getMethods().filter(method => method.getName() === 'u3')[0];
    const problem = new UndefinedVariableChecker([...u3.getCfg()!.getBlocks()][0].getStmts()[u3.getParameters().length], u3);
    const solver = new UndefinedVariableSolver(problem, scene);
    solver.solve();
    it('case2: UndefinedVariable', () => {
        expect(problem.getOutcomes().length).eq(1);
    });
});

describe('UndefinedVariableTest4', () => {
    const u4 = scene.getMethods().filter(method => method.getName() === 'u4')[0];
    const problem = new UndefinedVariableChecker([...u4.getCfg()!.getBlocks()][0].getStmts()[u4.getParameters().length], u4);
    const solver = new UndefinedVariableSolver(problem, scene);
    solver.solve();
    it('case2: UndefinedVariable', () => {
        expect(problem.getOutcomes().length).eq(1);
    });
});