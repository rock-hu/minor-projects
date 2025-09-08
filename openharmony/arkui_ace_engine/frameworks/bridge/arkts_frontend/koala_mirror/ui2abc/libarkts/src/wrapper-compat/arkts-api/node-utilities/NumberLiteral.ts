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

import { NumberLiteral } from '../types';
import { isSameNativeObject } from '../../../arkts-api/peers/ArktsObject';
import { attachModifiers, compose, updateThenAttach } from '../utilities/private';

export function updateNumberLiteral(original: NumberLiteral, value: number): NumberLiteral {
    if (isSameNativeObject(value, original.value)) {
        return original;
    }

    const update = updateThenAttach(
        compose(NumberLiteral.create), // Improve: No UpdateNumberLiteral, need to change this
        attachModifiers
    );
    return update(original, value);
}
