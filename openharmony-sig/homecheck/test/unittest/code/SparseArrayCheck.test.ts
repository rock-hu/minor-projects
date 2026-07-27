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
import SparseArrayCheck from 'src/checkers/performance/SparseArrayCheck';

let issueReports: IssueReport[] = [];
let realPath: string = '';

beforeAll(() => {
    const testPath = './test/unittest/sample/SparseArray';
    const myScene = getScene(testPath, true);
    realPath = myScene.getRealProjectDir();
    issueReports = new SparseArrayCheck().check(myScene, '');
})


describe('SparseArrayCheckTest', () => {

    /**
     * @tc.number: SparseArrayCheckTest_001
     * @tc.name: 稀疏数组需要上报
     * @tc.desc: 稀疏数组需要上报
     */
    test('SparseArrayCheckTest_001', () => {
        const detectFile: string = path.join(realPath, 'ets', 'SparseArrayReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => (issueReport.filePath === detectFile) 
        && (issueReport.defect.fixKey.includes('16%31%34') || issueReport.defect.fixKey.includes('19%6%9')));
        expect(detectFileReports.length).toBe(2);
    })

    /**
     * @tc.number: SparseArrayCheckTest_002
     * @tc.name: 稀疏数组不需要上报
     * @tc.desc: 稀疏数组不需要上报
     */
    test('SparseArrayCheckTest_002', () => {
        const detectFile: string = path.join(realPath, 'ets', 'SparseArrayNoReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        expect(detectFileReports.length).toBe(0);
    })

})