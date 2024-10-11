/**
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

export const tsInt = 1;
export const notFound = 'Not found';

interface SimpleObjectInterface {
    [key: string]: number | number[] | (() => number);
}

type AnyType = {};

export const handler = {
    get: function (target, property): AnyType {
        if (!isNaN(property)) {
            return target.items[property];
        }

        if (typeof target[property] === 'function') {
            return target[property]();
        }

        if (target[property]) {
            return target[property];
        }

        return notFound;
    },
    set: function (target, property, value): boolean {

        if (!isNaN(property)) {
            target.items[property] = value;
            return true;
        }

        target[property] = value;
        return true;
    }
};

export const SimpleObject: SimpleObjectInterface = {
    int: tsInt,
    items: [],
    add: (): number => tsInt + tsInt,
};
