/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { RootState } from '..';
import { ICodeReq } from '../../models/code';

export const selectCompileLoading = (state: RootState): boolean => state.code.isCompileLoading;
export const selectRunLoading = (state: RootState): boolean => state.code.isRunLoading;
export const selectShareLoading = (state: RootState): boolean => state.code.isShareLoading;
export const selectCode = (state: RootState): string => state.code.code;
export const selectCompileRes = (state: RootState): ICodeReq | null => state.code.compileRes;
export const selectRunRes = (state: RootState): ICodeReq | null => state.code.runRes;
