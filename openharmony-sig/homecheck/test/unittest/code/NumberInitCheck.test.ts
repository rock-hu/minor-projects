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
import NumberInitCheck from 'src/checkers/performance/NumberInitCheck';

let issueReports: IssueReport[] = [];
let realPath: string = '';

beforeAll(() => {
    const testPath = './test/unittest/sample/NumberInit';
    const myScene = getScene(testPath, true);
    realPath = myScene.getRealProjectDir();
    issueReports = new NumberInitCheck().check(myScene, '');
})


describe('NumberInitCheckTest', () => {

    /**
     * @tc.number: NumberInitCheckTest_001
     * @tc.name: 变量的定义语句和重新赋值类型不一样
     * @tc.desc: 变量的定义语句和重新赋值类型不一样
     */
    test('NumberInitCheckTest_001', () => {
        const detectFile: string = path.join(realPath, 'ets', 'NumberInitReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => (issueReport.filePath === detectFile) 
        && (issueReport.defect.fixKey.includes('19%5%5') || issueReport.defect.fixKey.includes('21%5%5') || 
        issueReport.defect.fixKey.includes('28%7%7') || issueReport.defect.fixKey.includes('24%5%11') || 
        issueReport.defect.fixKey.includes('33%7%9') || issueReport.defect.fixKey.includes('30%5%5') || 
        issueReport.defect.fixKey.includes('37%5%5') || issueReport.defect.fixKey.includes('40%5%5')));
        expect(detectFileReports.length).toBe(8);
    })

    /**
     * @tc.number: NumberInitCheckTest_002
     * @tc.name: 变量的定义语句和重新赋值类型一样或类型无法获取
     * @tc.desc: 变量的定义语句和重新赋值类型一样或类型无法获取
     */
    test('NumberInitCheckTest_002', () => {
        const detectFile: string = path.join(realPath, 'ets', 'NumberInitNoReport.ets');
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        expect(detectFileReports.length).toBe(0);
    })

})