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

import { useEffect } from 'react';

export const useClickOutside = (ref: React.RefObject<HTMLElement>, onClickOutside: () => void): void => {
    useEffect(() => {
        const handleClickOutside = (event: MouseEvent): void => {
            if (ref.current && !ref.current.contains(event.target as Node)) {
                onClickOutside();
            }
        };

        document.addEventListener('mousedown', handleClickOutside);

        return (): void => {
            document.removeEventListener('mousedown', handleClickOutside);
        };
    }, [ref, onClickOutside]);
};
