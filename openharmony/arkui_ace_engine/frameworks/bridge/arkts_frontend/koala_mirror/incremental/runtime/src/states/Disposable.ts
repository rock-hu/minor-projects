/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

/**
 * A marker for objects that require some work done for cleaning up.
 */
export interface Disposable {
    /**
     * Returns `true` if the object should no longer be used.
     */
    readonly disposed: boolean
    /**
     * Invoked when all resources owned by the object can safely be released.
     * Once disposed of the object should no longer be considered to be usable.
     */
    dispose(): void
}

/**
 * @param array - an array whose elements will be disposed from the first to the last
 */
export function disposeContentForward<Type extends Disposable>(array: ReadonlyArray<Type | undefined>) {
    const length = array.length
    for (let i = 0; i < length; i++) {
        array[i]?.dispose()
    }
}

/**
 * @param array - an array whose elements will be disposed from the last to the first
 */
export function disposeContentBackward<Type extends Disposable>(array: ReadonlyArray<Type | undefined>) {
    let i = array.length
    while (0 < i--) {
        array[i]?.dispose()
    }
}

/**
 * @param it - an iterator that provides elements to dispose one by one
 */
export function disposeContent<Type extends Disposable>(it: IterableIterator<Type | undefined>) {
    while (true) {
        const result = it.next()
        if (result.done) break
        result.value?.dispose()
    }
}
