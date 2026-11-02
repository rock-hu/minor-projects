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

import { assert, beforeAll, describe, test } from 'vitest';
import { getScene } from './common/testCommon';
import { IssueReport } from '../../../src/object/IssueReport';
import path from 'path';
import CacheAvplayerCheck from 'src/checkers/performance/CacheAvplayerCheck';

let issueReports: IssueReport[] = [];
let realPath: string = '';

beforeAll(() => {
    const testPath = './test/unittest/sample/CacheAvplayer';
    const myScene = getScene(testPath, false);
    realPath = myScene.getRealProjectDir();
    issueReports = new CacheAvplayerCheck().check(myScene, '');
})

describe('CacheAvplayerTest', () => {
    test('CacheAvplayerTest_001', () => {
        const detectFile: string = path.join(realPath, 'ets', 'CacheAvplayerNoReport.ets');
        const detectedFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        assert.equal(detectedFileReports.length, 0, 'The number of reported line should be 0.');
    })

    test('CacheAvplayerTest_002', () => {
        const detectFile: string = path.join(realPath, 'ets', 'CacheAvplayerReport.ets');
        const expectReportList = ['32%21%27%'];
        const detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        assert.equal(detectFileReports.length, expectReportList.length, 'The number of reported line is different from the expected number of line.');
        detectFileReports.forEach((detectFileReport, index) => {
            assert.include(detectFileReport.defect.fixKey, expectReportList[index]);
        });
    });
})