/**
 * 最佳实践：资源泄漏类问题优化建议
 */

// [Start resource_leak_advise2]
static bool InjectNativeLeak()
{
    char* p = (char*)malloc(g_cmdLen + 1);
    if (!p) {
        return false;
    }
    int err = memset_s(p, g_cmdLen + 1, 'a', g_cmdLen);
    if (err) {
        free(p);
        return false;
    }
    free(p);
    return true;
}
// [End resource_leak_advise2]

/**
 * 最佳实践：资源泄漏类问题优化建议
 */

// [Start resource_leak_advise3]
void processWithLeak(int fd, size_t size) {
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        return;
    }
    // 使用共享内存
    processData(ptr);
    //忘记调用 munmap(ptr, size);
    // 每一次调用都会泄漏一块映射内存
}
// [End resource_leak_advise3

/**
 * 最佳实践：资源泄漏类问题优化建议
 */

// [Start resource_leak_advise4]
void InjectContinuingFileFdLeak(string path) {
    mode_t fileMode = 0644;
    int fd = open(path, O_CREAT | O_RDW, fileMode);
    if (fd < 0) {
        break;
    }

    if (xxx) {
        close(fd); // 异常分支处理
        return;
    }
    close(fd); // 正常业务流程关闭句柄
}
// [End resource_leak_advise4]

/**
 * 最佳实践：资源泄漏类问题优化建议
 */

// [Start resource_leak_advise5]
void riskyThreadFunction() {
    std::thread t([]() {
        // 长时间运行的任务
    });
    t.detach(); // 危险！失去对线程的控制
}
// [End resource_leak_advise5]