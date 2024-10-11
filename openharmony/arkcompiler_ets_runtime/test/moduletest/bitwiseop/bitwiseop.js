/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

/*
 * @tc.name:bitwiseop
 * @tc.desc:test bitwise operation
 * @tc.type: FUNC
 * @tc.require: issueI5NO8G
 */
print(13 & 3)
print(13 | 3)
print(13 ^ 3)
print(13 << 3)
print(13 >> 3)
print(13 >>> 3)
print(0 | -1)
print(1 & -1)
print(0 ^ -1)
print(-1 >>> 0)
print(~13)
