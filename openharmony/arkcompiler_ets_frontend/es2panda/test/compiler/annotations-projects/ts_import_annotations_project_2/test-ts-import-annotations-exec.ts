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

import { Anno1, Anno2 } from "./test-ts-export-annotations"

@__$$ETS_ANNOTATION$$__Anno1
class A {}

@__$$ETS_ANNOTATION$$__Anno1()
class B {}

@__$$ETS_ANNOTATION$$__Anno1({})
class C {}

@__$$ETS_ANNOTATION$$__Anno2({a: 1, b: "string"})
class D {}