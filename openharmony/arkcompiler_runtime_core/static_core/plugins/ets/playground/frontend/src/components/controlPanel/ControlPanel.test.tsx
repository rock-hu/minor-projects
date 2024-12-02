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
import { render, screen, fireEvent } from '@testing-library/react';
import ControlPanel from './ControlPanel';
import { Provider } from 'react-redux';
import configureMockStore from 'redux-mock-store';
import { AppDispatch } from '../../store';

const mockStore = configureMockStore<AppDispatch>();

describe('ControlPanel component', () => {
    let store: ReturnType<typeof mockStore>;

    beforeEach(() => {
        store = mockStore({
        // @ts-ignore
            appState: { disasm: false },
            code: { compileLoading: false, runLoading: false },
            options: { isLoading: false, options: [], pickedOptions: [] }
        });
        store.dispatch = jest.fn();
    });

    const renderWithProviders = (component: JSX.Element): ReturnType<typeof render> => {
        return render(<Provider store={store}>{component}</Provider>);
    };

    it('renders the ControlPanel component with all buttons and elements', () => {
        renderWithProviders(<ControlPanel />);
        expect(screen.getByText('Compile')).toBeInTheDocument();
        expect(screen.getByText('Run')).toBeInTheDocument();
        expect(screen.getByText('Compile options')).toBeInTheDocument();
    });

    it('dispatches compile action on Compile button click', () => {
        renderWithProviders(<ControlPanel />);
        const compileButton = screen.getByText('Compile');
        fireEvent.click(compileButton);
        expect(store.dispatch).toHaveBeenCalled();
    });

    it('dispatches run action on Run button click', () => {
            renderWithProviders(<ControlPanel />);
        const runButton = screen.getByText('Run');
        fireEvent.click(runButton);
        expect(store.dispatch).toHaveBeenCalled();
    });

    it('toggles disasm checkbox and dispatches action', () => {
        renderWithProviders(<ControlPanel />);
        const disasmCheckbox = screen.getByTestId('disasm-checkbox');
        fireEvent.click(disasmCheckbox);
        expect(store.dispatch).toHaveBeenCalled();
    });

    it('opens and closes the Compile Options popover on click', () => {
        renderWithProviders(<ControlPanel />);
        const compileOptionsButton = screen.getByText('Compile options');
        fireEvent.click(compileOptionsButton);
        expect(screen.getByTestId('compile-options-content')).toBeInTheDocument();
    });

    it('disables Compile button when compile is loading', () => {
        store = mockStore({
            // @ts-ignore
            appState: { disasm: false },
            options: { isLoading: false, options: [], pickedOptions: [] },
            code: { isCompileLoading: true, isRunLoading: false },
        });
        renderWithProviders(<ControlPanel />);
        const compileButton = screen.getByText('Compile');
        expect(compileButton).toBeDisabled();
    });

    it('disables Run button when run is loading', () => {
        store = mockStore({
            // @ts-ignore
            appState: { disasm: false },
            options: { isLoading: false, options: [], pickedOptions: [] },
            code: { isCompileLoading: false, isRunLoading: true },
        });
        renderWithProviders(<ControlPanel />);
        const runButton = screen.getByText('Run');
        expect(runButton).toBeDisabled();
    });
});
