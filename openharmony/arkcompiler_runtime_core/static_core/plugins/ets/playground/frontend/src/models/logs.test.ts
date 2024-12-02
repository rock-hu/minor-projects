/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import configureMockStore from 'redux-mock-store';
import { handleResponseLogs } from './logs';
import {AppDispatch} from '../store';

const mockStore = configureMockStore<AppDispatch>();

describe('handleResponseLogs thunk', () => {
    let store: ReturnType<typeof mockStore>;
    const initialState = {
        logs: {
            compileErr: [],
            compileOut: [],
            disasmErr: [],
            disasmOut: [],
            err: [],
            out: [],
            runErr: [],
            runOut: []
        },
    };

    beforeEach(() => {
        // @ts-ignore
        store = mockStore(initialState);
        jest.clearAllMocks();
    });

    it('dispatches compile error log correctly', async () => {
        const apiResponse = {
            data: {
                compile: {
                    exit_code: 1,
                    error: 'Compile error occurred'
                }
            }
        };

        const dispatch = jest.fn();

        await handleResponseLogs(apiResponse);
    });
});