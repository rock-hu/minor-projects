/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include <pthread.h>
#include <cstdlib>
#define MC_ON
#include "platforms/unix/libpandabase/futex/fmutex.cpp"

// The tests checks mutex try lock

// Copy of mutex storage, after complete implementation should totally replace mutex::current_tid
thread_local pthread_t current_tid;

extern "C" void __VERIFIER_assume(int) __attribute__((__nothrow__));

static struct fmutex g_x;
static int g_shared;

static void *ThreadN(void *arg)
{
    intptr_t index = reinterpret_cast<intptr_t>(arg);

    bool ret = MutexLock(&g_x, true);
    __VERIFIER_assume(ret);
    g_shared = index;
    int r = g_shared;
    ASSERT(r == index);
    MutexUnlock(&g_x);
    return nullptr;
}

int main()
{
    constexpr int N = 2;
    MutexInit(&g_x);
    pthread_t t[N];

    for (long i = 0u; i < N; i++) {
        pthread_create(&t[i], nullptr, ThreadN, reinterpret_cast<void *>(i));
    }

    for (int i = 0u; i < N; i++) {
        pthread_join(t[i], nullptr);
    }

    MutexDestroy(&g_x);
    return 0;
}
