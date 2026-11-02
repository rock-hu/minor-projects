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

import { assert, beforeAll, describe, expect, test } from 'vitest';
import { getScene } from './common/testCommon';
import { IssueReport } from '../../../src/object/IssueReport';
import path from 'path';
import TimezoneInterfaceCheck from 'src/checkers/performance/TimezoneInterfaceCheck';

let issueReports: IssueReport[] = [];
let realPath: string = '';

beforeAll(() => {
    const testPath = './test/unittest/sample/TimezoneInterface';
    const myScene = getScene(testPath, false);
    realPath = myScene.getRealProjectDir();
    issueReports = new TimezoneInterfaceCheck().check(myScene, '');
})

describe('TimezoneInterfaceCheckTest', () => {
    test('TimezoneInterfaceCheckTest_001', () => {
        const detectFile: string = path.join(realPath, 'ets', 'TimezoneNoReport1.ets');
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        assert.equal(detectFileReports.length, 0, 'The number of reported line should equal 0.');
    })

    test('TimezoneInterfaceCheckTest_002', () => {
        const detectFile: string = path.join(realPath, 'ets', 'TimezoneNoReport2.ets');
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        assert.equal(detectFileReports.length, 0, 'The number of reported line should equal 0.');
    })

    test('TimezoneInterfaceCheckTest_003', () => {
        const detectFile: string = path.join(realPath, 'ets', 'TimezoneNoReport3.ets');
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        assert.equal(detectFileReports.length, 0, 'The number of reported line should equal 0.');
    })

    test('TimezoneInterfaceCheckTest_004', () => {
        const detectFile: string = path.join(realPath, 'ets', 'TimezoneReport1.ets');
        let detectFileReports = issueReports.filter((issueReport) => (issueReport.filePath === detectFile)
        && issueReport.defect.fixKey.includes('21%15%25'));
        expect(detectFileReports.length).toBe(1);
    })

    test('TimezoneInterfaceCheckTest_005', () => {
        const detectFile: string = path.join(realPath, 'ets', 'TimezoneReport2.ets');
        let expectReportList = ['18%10%18', '19%10%18', '20%10%18', '21%10%18'];
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        assert.equal(detectFileReports.length, expectReportList.length,
            'The number of reported line is different from the expected number of line.');
        detectFileReports.forEach((detectFileReport, index) => {
            assert.include(detectFileReport.defect.fixKey, expectReportList[index]);
        });
    })
})