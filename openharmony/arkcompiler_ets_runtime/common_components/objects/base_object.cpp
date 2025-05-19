/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "common_interfaces/objects/base_object.h"

namespace panda {
#ifdef USE_CMC_GC
BaseObjectOperator BaseObject::operator_;

void BaseObject::RegisterDynamic(BaseObjectOperatorInterfaces *dynamicObjOp)
{
    operator_.dynamicObjOp_ = dynamicObjOp;
}

void BaseObject::RegisterStatic(BaseObjectOperatorInterfaces *staticObjOp)
{
    operator_.staticObjOp_ = staticObjOp;
}
#endif
}  // namespace panda
