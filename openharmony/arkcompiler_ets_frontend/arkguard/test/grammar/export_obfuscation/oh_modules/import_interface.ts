/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { publicFoo1 as publicFoo1Alias} from './export_interface';
import { publicFoo2 } from './export_interface';
import { publicFoo3Alias as publicFoo3 } from './export_interface';
import { publicFoo4 } from './export_interface';
import defaultValueExport from './export_interface';
import { default as defaultExport} from './export_interface'

publicFoo1Alias();
publicFoo2();
publicFoo3();
publicFoo4();
defaultValueExport;
defaultExport;