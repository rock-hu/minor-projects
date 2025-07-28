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

/**
 * 最佳实践：地址越界经典问题类型
 * heap-use-after-free
 */

// [Start heap-use-after-free]
#include <stdlib.h>
int main()
{
    int *array = new int[100];
    delete[] array;
    return array[5];
}
// [End heap-use-after-free]

/**
 * 最佳实践：地址越界经典问题类型
 * stack-user-after-return
 */

// [Start stack-user-after-return]
int *ptr;

__attribute__((noinline))
void FunctionThatEscapesLocalObject()
{
    int local[100];
    ptr = &local[0];
}
int main(int argc, char** argv) {
    FunctionThatEscapesLocalObject();
    return ptr[argc];
}
// [End stack-user-after-return]

/**
 * 最佳实践：地址越界经典问题类型
 * heap-buffer-overflow
 */

// [Start heap-buffer-overflow]
int heapBufferOverflow()
{
    char* buffer;
    buffer = (char *)malloc(100);
    *(buffer + 101) = 'n';
    *(buffer + 102) = 'n';
    free(buffer);
    return buffer[1];
}
// [End heap-buffer-overflow]

/**
 * 最佳实践：地址越界类问题分析方法
 */

// [Start address_overflow_code]
int main(int argc, char** argv) {
{
    int stack_array[99];
    stack_array[1] = 0;
    return stack_array[argc + 100]; // BOOM
}
// [End address_overflow_code]

/**
 * 最佳实践：地址越界优化建议
 */

// [Start address_sanitizer_advise1_negative]
std::string GetStringParam(napi_env env, napi_value arg)
{
    size_t size;
    napi_get_value_string_utf8(env, arg, nullptr, 0, &size);
    size_t str_size = size + 1;
    char *buf = new char[str_size];
    napi_get_value_string_utf8(env, arg, buf, str_size, nullptr); // 未进行校验
    std::string str(buf);
    delete[] buf;
    return str;
}
// [End address_sanitizer_advise1_negative]

/**
 * 最佳实践：地址越界优化建议
 */

// [Start address_sanitizer_advise1_positive]
std::string GetStringParam(napi_env env, napi_value arg)
{
    size_t size;
    // 增加校验
    if (napi_get_value_string_utf8(env, arg, nullptr, 0, &size) != napi_ok || size == 0) {
        return "";
    }
    size_t str_size = size + 1;
    char *buf = new char[str_size];
    // 增加校验
    if (napi_get_value_string_utf8(env, arg, buf, str_size, nullptr) != napi_Ok) {
        delete[] buf;
        return "";
    }
    std::string str(buf);
    delete[] buf;
    return str;
}
// [End address_sanitizer_advise1_positive]

/**
 * 最佳实践：地址越界优化建议
 */

// [Start address_sanitizer_advise2_negative]
class Task {
public:
    void Start() {
        std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            DoWork(); // 如果 Task 已析构，这里是悬空调用
        }).detach();
    }

    void DoWork() {
        std::cout << "Doing work\n";
    }

    ~Tash() {
        std::cout << "Task destructed\n";
    }
};

void Run() {
    Task* task = new Task();
    task->Start();
    delete task; // 析构发生在 lambda 执行前
    std::thread::sleep_for(std::chrono::milliseconds(1));
}
// [End address_sanitizer_advise2_negative]

// [Start address_sanitizer_advise2_positive1]
class Task : public std::enable_shared_from_this<Task> {
    // ...
}
// [End address_sanitizer_advise2_positive1]

// [Start address_sanitizer_advise2_positive2]
auto task = std::make_shared<Task>();
// [End address_sanitizer_advise2_positive2]

// [Start address_sanitizer_advise2_positive3]
void Start() {
    auto weak = weak_from_this();
    std::thread([weak]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto strong = weak.lock();
        if (!strong) {
            return;
        }
        strong->DoWrok();
    }).detach();
}
// [End address_sanitizer_advise2_positive3]

/**
 * 最佳实践：地址越界优化建议
 */

// [Start address_sanitizer_advise3_negative]
std::map<int, int*> myMap;

void Erase(int key) {
    auto it = myMap.find(key);
    if (it != myMap.end()) {
        delete it->second;
        myMap.erase(it); // 多线程下未加锁，可能重复删除
    }
}

int main() {
    myMap[1] = new int(100);
    std::thread t1(Erase, 1);
    std::thread t2(Erase, 1); // 两个线程同时删除同一个 key
    t1.join();
    t2.join();
    return 0;
}
// [End address_sanitizer_advise3_negative]

// [Start address_sanitizer_advise3_positive]
std::map<int, int*> myMap;
std::mutex mtx;
void Erase(int key) {
    std::lock_guard<std::mutex> lock(mtx); // 加锁保护
    auto it = myMap.find(key);
    if (it != myMap.end()) {
        delete it->second;
        myMap.erase(it); // 多线程下未加锁，可能重复删除
    }
}

int main() {
    myMap[1] = new int(100);
    std::thread t1(Erase, 1);
    std::thread t2(Erase, 1); // 两个线程同时删除同一个 key
    t1.join();
    t2.join();
    return 0;
}
// [End address_sanitizer_advise3_positive]

/**
 * 最佳实践：地址越界优化建议
 */

// [Start address_sanitizer_advise4_negative]
int* g_ptr = nullptr;

void WorkerThread() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    *g_ptr = 24; // 子线程访问已释放的对象，UAF 问题
}

int main() {
    g_ptr = new int(42);
    std::thread t(WorkerThread);
    delete g_ptr; // 主线程提前释放
    t.join();
    return 0;
}
// [End address_sanitizer_advise4_negative]

// [Start address_sanitizer_advise4_positive]
int* g_ptr = nullptr;

void WorkerThread() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (g_ptr != nullptr) { // 判空处理
        *g_ptr = 24;
    }
}

int main() {
    g_ptr = new int(42);
    std::thread t(WorkerThread);
    delete g_ptr; // 主线程提前释放
    g_ptr = nullptr; // 指针置空
    t.join();
    return 0;
}
// [End address_sanitizer_advise4_positive]