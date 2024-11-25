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

export interface ITokenizer {
    root: (string | RegExp)[][];
}

export interface ISyntax {
    tokenizer: ITokenizer;
}

type Convertible = string | RegExp | { [key: string]: Convertible } | Convertible[];

const convertRegexStrings = (obj: Convertible): Convertible => {
    if (Array.isArray(obj)) {
        return obj.map(item => convertRegexStrings(item));
    } else if (typeof obj === 'object' && obj !== null && !(obj instanceof RegExp)) {
        const result: { [key: string]: Convertible } = {};
        for (const key in obj) {
            if (Object.prototype.hasOwnProperty.call(obj, key)) {
                result[key] = convertRegexStrings(obj[key]);
            }
        }
        return result;
    } else if (typeof obj === 'string') {
        if (obj.startsWith('/') && obj.endsWith('/')) {
            return new RegExp(obj.slice(1, -1));
        }
        return obj;
    } else {
        return obj;
    }
};

export const syntaxModel = {
    // @ts-ignore
    fromApi: (data: ISyntax): ISyntax => convertRegexStrings(data) as ISyntax
};
