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

#include "core/components/video/video_component_v2.h"

#include "core/components/video/video_element_v2.h"

namespace OHOS::Ace {

RefPtr<Element> VideoComponentV2::CreateElement()
{
    return AceType::MakeRefPtr<VideoElementV2>();
}

} // namespace OHOS::Ace
