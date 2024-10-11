/**
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_PLUGINS_ETS_COMPILER_ETS_CODEGEN_EXTENSIONS_H
#define PANDA_PLUGINS_ETS_COMPILER_ETS_CODEGEN_EXTENSIONS_H

bool LaunchCallCodegen(CallInst *inst);
// CC-OFFNXT(G.NAM.01) false positive
void PrepareForCallLaunchVirtual(CallInst *callInst, RuntimeInterface::MethodPtr method, Reg &thisReg, Reg &param0);

#endif  // PANDA_PLUGINS_ETS_COMPILER_ETS_CODEGEN_EXTENSIONS_H
