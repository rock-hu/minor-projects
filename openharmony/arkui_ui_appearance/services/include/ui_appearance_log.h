/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef UI_APPEARANCE_LOG_H
#define UI_APPEARANCE_LOG_H

#include "hilog/log.h"

#define UIAPPERANCE_FILE_NAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define PRINT_LOG(level, fmt, ...) \
    HILOG_IMPL(LOG_CORE, LOG_##level, 0xD003900, "UiAppearance", "[%{public}s:%{public}s:%{public}d]" fmt, \
        UIAPPERANCE_FILE_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define LOGE(fmt, ...) PRINT_LOG(ERROR, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) PRINT_LOG(WARN, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) PRINT_LOG(INFO, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) PRINT_LOG(DEBUG, fmt, ##__VA_ARGS__)

#endif // UI_APPEARANCE_LOG_H
