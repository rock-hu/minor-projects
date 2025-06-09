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

import React from 'react';
import {fireEvent, render, screen} from '@testing-library/react';
import LogsView from './LogsView';
import {ELogType, ILog} from '../../models/logs';
import { AppDispatch } from '../../store';
import { Provider } from 'react-redux';
import { ThemeProvider } from '../../components/theme/ThemeContext';
import configureMockStore from 'redux-mock-store';

const mockClearFilters = jest.fn();
const mockStore = configureMockStore<AppDispatch>();

const mockLogs: ILog[] = [
    { message: 'This is a log message', from: ELogType.RUN_OUT },
    { message: 'This is an error message', from: ELogType.RUN_ERR },
];

describe('LogsView component', () => {
    let store: ReturnType<typeof mockStore>;

    beforeEach(() => {
        store = mockStore({
            // @ts-ignore
            appState: { theme: 'light', primaryColor: '#e32b49', disasm: false, clearLogsEachRun: true },
        });
        store.dispatch = jest.fn();
    });

    const renderWithProviders = (component: JSX.Element): ReturnType<typeof render> => {
        return render(
            <Provider store={store}>
                <ThemeProvider>{component}</ThemeProvider>
            </Provider>
        );
    };
    beforeEach(() => {
        jest.clearAllMocks();
    });

    it('renders log messages', () => {
        renderWithProviders(<LogsView logArr={mockLogs} clearFilters={mockClearFilters} logType='out' />);

        expect(screen.getByText('[LOG]:')).toBeInTheDocument();
        expect(screen.getByText('[ERR]:')).toBeInTheDocument();
        expect(screen.getByText(mockLogs[0].message)).toBeInTheDocument();
        expect(screen.getByText(mockLogs[1].message)).toBeInTheDocument();
    });

    it('filters log messages based on input', () => {
        renderWithProviders(<LogsView logArr={mockLogs} logType='out' clearFilters={mockClearFilters} />);

        const filterInput = screen.getByPlaceholderText('Filter');
        fireEvent.change(filterInput, { target: { value: 'error' } });

        expect(screen.queryByText(mockLogs[0].message)).not.toBeInTheDocument();
        expect(screen.getByText(mockLogs[1].message)).toBeInTheDocument();
    });

    it('resets log messages on empty input', () => {
        renderWithProviders(<LogsView logArr={mockLogs} logType='out' clearFilters={mockClearFilters} />);

        const filterInput = screen.getByPlaceholderText('Filter');
        fireEvent.change(filterInput, { target: { value: 'log' } });
        expect(screen.getByText(mockLogs[0].message)).toBeInTheDocument();
        expect(screen.queryByText(mockLogs[1].message)).not.toBeInTheDocument();

        fireEvent.change(filterInput, { target: { value: '' } });
        expect(screen.getByText(mockLogs[0].message)).toBeInTheDocument();
        expect(screen.getByText(mockLogs[1].message)).toBeInTheDocument();
    });

    it('calls clearFilters when clear icon is clicked', () => {
        renderWithProviders(<LogsView logArr={mockLogs} logType='out' clearFilters={mockClearFilters} />);

        const clearIcon = screen.getByTestId('clear-icon');
        fireEvent.click(clearIcon);

        expect(mockClearFilters).toHaveBeenCalled();
    });
});
