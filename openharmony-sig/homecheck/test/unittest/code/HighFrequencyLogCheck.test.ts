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
import HighFrequencyLogCheck from 'src/checkers/performance/HighFrequencyLogCheck';

let issueReports: IssueReport[] = [];
let realPath: string = '';

beforeAll(() => {
    const testPath = './test/unittest/sample/HighFrequencyLog';
    const myScene = getScene(testPath, true);
    realPath = myScene.getRealProjectDir();
    issueReports = new HighFrequencyLogCheck().check(myScene, '');
})


describe('HighFrequencyLogCheckTest', () => {

    /**
     * @tc.number: HighFrequencyLogCheck_001
     * @tc.name: 在onWillScroll中使用日志打印
     * @tc.desc: 在onWillScroll中使用日志打印
     */
    test('HighFrequencyLogCheck_001', () => {
        const detectFile: string = path.join(realPath, 'ets', 'HighFrequencyLogReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => (issueReport.filePath === detectFile) 
        && (issueReport.defect.fixKey.includes('31%9%23') || issueReport.defect.fixKey.includes('32%13%21') || 
        issueReport.defect.fixKey.includes('33%9%23') || issueReport.defect.fixKey.includes('33%9%23') || 
        issueReport.defect.fixKey.includes('37%9%13')));
        expect(detectFileReports.length).toBe(5);
    })

    /**
     * @tc.number: HighFrequencyLogCheck_002
     * @tc.name: 未在高频函数中打印日志
     * @tc.desc: 未在高频函数中打印日志
     */
    test('HighFrequencyLogCheck_002', () => {
        const detectFile: string = path.join(realPath, 'ets', 'HighFrequencyLogNoReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        expect(detectFileReports.length).toBe(0);
    })

})