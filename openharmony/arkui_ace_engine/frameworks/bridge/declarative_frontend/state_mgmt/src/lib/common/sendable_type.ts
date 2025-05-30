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

declare class SendableMap<K, V> {
    keys(): IterableIterator<K>;
    values(): IterableIterator<V>;
};
declare class SendableSet<T> {
    values(): IterableIterator<T>;
};
declare class SendableArray {};

class SendableType {
    public static isArray<T extends object>(o: T): boolean {
        return o instanceof SendableArray;
    }
    public static isSet<T extends object>(o: T): boolean {
        return o instanceof SendableSet;
    }

    public static isMap<T extends object>(o: T): boolean {
        return o instanceof SendableMap;
    }

    public static isContainer<T extends object>(o: T): boolean {
        return o instanceof SendableMap || o instanceof SendableSet || o instanceof SendableArray;
    }
}