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

import { beforeAll, describe, expect, test } from 'vitest';
import { getScene } from './common/testCommon';
import { IssueReport } from '../../../src/object/IssueReport';
import path from 'path';
import ArrayDefinitionCheck from 'src/checkers/performance/ArrayDefinitionCheck';

let issueReports: IssueReport[] = [];
let realPath: string = '';

beforeAll(() => {
    const testPath = './test/unittest/sample/ArrayDefinition';
    const myScene = getScene(testPath, false);
    realPath = myScene.getRealProjectDir();
    issueReports = new ArrayDefinitionCheck().check(myScene, '');
})


describe('ArrayDefinitionCheckTest', () => {

    /**
     * @tc.number: ArrayDefinitionCheckTest_001
     * @tc.name: 数字与字符串数组格式定义错误
     * @tc.desc: 数字与字符串数组格式定义错误
     */
    test('ArrayDefinitionCheckTest_001', () => {
        const detectFile: string = path.join(realPath, 'ets', 'ArrayDefinitionReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => (issueReport.filePath === detectFile) 
        && (issueReport.defect.fixKey.includes('16%11%24') || issueReport.defect.fixKey.includes('17%11%24')));
        expect(detectFileReports.length).toBe(2);
    })

    /**
     * @tc.number: ArrayDefinitionCheckTest_002
     * @tc.name: 数字与字符串数组格式定义正确
     * @tc.desc: 数字与字符串数组格式定义正确
     */
    test('ArrayDefinitionCheckTest_002', () => {
        const detectFile: string = path.join(realPath, 'ets', 'ArrayDefinitionNoReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        expect(detectFileReports.length).toBe(0);
    })

})