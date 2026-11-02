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
import StartWindowIconCheck from 'src/checkers/performance/StartWindowIconCheck';

let issueReports: IssueReport[] = [];
let realPath: string = '';

beforeAll(() => {
    const testPath = './test/unittest/sample/StartWindowIcon';
    const myScene = getScene(testPath, false);
    realPath = myScene.getRealProjectDir();
    issueReports = new StartWindowIconCheck().check(myScene, '');
})

describe('StartWindowIconCheckTest', () => {

    /**
     * @tc.number: StartWindowIconCheckTest_001
     * @tc.name: startWindowIcon图标尺寸大于256*256
     * @tc.desc: startWindowIcon图标尺寸大于256*256
     */
    test('StartWindowIconCheckTest_001', () => {
        const detectFile: string = path.join(realPath, 'ets', 'src', 'report', 'module.json5');
        let detectFileReports = issueReports.filter((issueReport) => (issueReport.filePath === detectFile) 
        && (issueReport.defect.fixKey.includes('22%27%42')));
        expect(detectFileReports.length).toBe(1);
    })

    /**
     * @tc.number: StartWindowIconCheckTest_002
     * @tc.name: startWindowIcon图标尺寸小于256*256
     * @tc.desc: startWindowIcon图标尺寸小于256*256
     */
    test('StartWindowIconCheckTest_002', () => {
        const detectFile: string = path.join(realPath, 'ets', 'src', 'noreport', 'module.json5');
        let detectFileReports = issueReports.filter((issueReport) => issueReport.filePath === detectFile);
        expect(detectFileReports.length).toBe(0);
    })

})