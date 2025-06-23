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
export const add: (a: number, b: number) => number;

// src/main/cpp/types/libentry/index.d.ts
// [Start native_add]
// Export the addition calculation APIs provided on the Native side
export const nativeAdd: (a: number, b: number) => number;
// [End native_add]

// src/main/cpp/types/libentry/index.d.ts
// [Start native_sub]
// Export the subtraction calculation interface provided by the Native side
export const nativeSub: (a: number, b: number, path: string) => number;
// [End native_sub]
