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

import { ClassSignature, MethodSignature } from '../../core/model/ArkSignature';

export function IsCollectionClass(classSignature: ClassSignature): boolean {
    if (classSignature.toString().endsWith('lib.es2015.collection.d.ts: Set') ||
        classSignature.toString().endsWith('lib.es2015.collection.d.ts: Map')) {
        return true;
    }
    return false;
}

export function IsCollectionAPI(method: MethodSignature): boolean {
    if (IsCollectionSetAdd(method) || IsCollectionMapSet(method)) {
        return true;
    }
    return false;
}

export function IsCollectionSetAdd(method: MethodSignature): boolean {
    if (method.toString().endsWith('lib.es2015.collection.d.ts: Set.add(T)')) {
        return true;
    }
    return false;
}

export function IsCollectionMapSet(method: MethodSignature): boolean {
    if (method.toString().endsWith('lib.es2015.collection.d.ts: Map.set(K, V)')) {
        return true;
    }
    return false;
}