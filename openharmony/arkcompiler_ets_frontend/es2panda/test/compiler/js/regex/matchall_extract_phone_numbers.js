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
const regex = /.\d{3}. \d{3}-\d{4}/g;
const str = "Call us at (123) 456-7890 or (987) 654-3210.";
const matches = [...str.matchAll(regex)];
print(JSON.stringify(matches)); // (123) 456-7890,(987) 654-3210
