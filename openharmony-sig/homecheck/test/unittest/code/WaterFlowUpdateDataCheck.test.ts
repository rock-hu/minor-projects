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
import WaterFlowUpdateDataCheck from 'src/checkers/performance/WaterFlowUpdateDataCheck';

let issueReports: IssueReport[] = [];
let realPath: string = '';

beforeAll(() => {
    const testPath = './test/unittest/sample/WaterFlowUpdateData';
    const myScene = getScene(testPath, false);
    realPath = myScene.getRealProjectDir();
    issueReports = new WaterFlowUpdateDataCheck().check(myScene, '');
})

describe('WaterFlowUpdateDataCheckTest', () => {

    /**
     * @tc.number: WaterFlowUpdateDataCheckTest_001
     * @tc.name: Waterflow未预加载数据
     * @tc.desc: Waterflow未预加载数据
     */
    test('WaterFlowUpdateDataCheckTest_001', () => {
        const detectFile: string = path.join(realPath, 'ets', 'WaterFlowUpdateDataReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => (issueReport.filePath === detectFile) 
        && (issueReport.defect.fixKey.includes('68%7%15')));
        expect(detectFileReports.length).toBe(1);
    })

    /**
     * @tc.number: WaterFlowUpdateDataCheckTest_002
     * @tc.name: Waterflow预加载数据
     * @tc.desc: Waterflow预加载数据
     */
    test('WaterFlowUpdateDataCheckTest_002', () => {
        const detectFile: string = path.join(realPath, 'ets', 'WaterFlowUpdateDataNoReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        expect(detectFileReports.length).toBe(0);
    })

})