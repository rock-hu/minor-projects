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

#ifndef FOUNDATION_ACE_COMPONENT_EXT_MOVINGPHOTO__UTILS_H
#define FOUNDATION_ACE_COMPONENT_EXT_MOVINGPHOTO__UTILS_H

#include <memory>
#include <unistd.h>

namespace OHOS::Ace {

// movingphoto playback mode
enum class PlaybackMode {
    NONE = 0,
    AUTO,
    REPEAT
};

// movingphoto pixelmap format
enum class MovingPhotoFormat {
    UNKNOWN = 0,
    RGBA_8888 = 1,
    NV21 = 2,
    RGBA_1010102 = 3,
    YCBCR_P010 = 4,
    YCRCB_P010 = 5
};

class SharedFd final {
public:
    SharedFd() = default;

    explicit SharedFd(int fd)
    {
        Reset(fd);
    }

    SharedFd& operator=(int fd)
    {
        return Reset(fd);
    }

    int GetValue() const
    {
        return (intptr_t)fd_.get();
    }
private:
    SharedFd& Reset(int fd)
    {
        fd_.reset((void*)(intptr_t)fd, fd == -1 ? [](void*) {} : [](void* fd) { close((intptr_t)fd); });
        return *this;
    }

    std::shared_ptr<void> fd_{(void*)(intptr_t)-1, [](void*) {}};
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_COMPONENT_EXT_MOVINGPHOTO__UTILS_H
