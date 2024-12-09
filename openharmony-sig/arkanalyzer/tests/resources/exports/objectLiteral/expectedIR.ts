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

import { ExportType } from '../../../../src/core/model/ArkExport';
import { ArkClass, DEFAULT, Local } from '../../../../src';

export const DefaultExportObjectLiteral_Expect_IR = {
    _default: true,
    nameBeforeAs: undefined,
    exportClauseName: DEFAULT,
    exportClauseType: ExportType.LOCAL,
    modifiers: ['DEFAULT'],
    arkExport: {
        type: Local,
        local: {
            type: ArkClass,
            classSignature: '@exports/objectLiteral/exportObjectLiteral.ts: %AC$%dflt$%dflt$0'
        }
    }
};
