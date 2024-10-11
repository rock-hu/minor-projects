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
#include <atomic>
#include <iostream>

#include "sync_api.h"

static std::atomic<bool> g_interruptFlag = false;
static std::atomic<bool> g_needInterrupt = false;

void ark::test::IntrusiveInterruptReleasePagesTestAPI::SetNeedInterrupt()
{
    g_needInterrupt = true;
}

void ark::test::IntrusiveInterruptReleasePagesTestAPI::WaitForThreadInterruption()
{
    if (g_needInterrupt) {
        while (!g_interruptFlag) {
        }
    }
}

void ark::test::IntrusiveInterruptReleasePagesTestAPI::NotifyAboutThreadInterruption()
{
    if (g_needInterrupt) {
        g_interruptFlag = true;
    }
}
