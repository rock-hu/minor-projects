/**
 * 最佳实践：使用Asan检测内存错误
 * 场景二：在app.json5中配置环境变量-配置Asan参数时
 */

// [Start set_asan]
allow_user_segv_handler=1
detect_odr_violation=0
alloc_dealloc_mismatch=0
allocator_may_return_null=1
detect_container_overflow=0
abort_on_error=0
halt_on_error=0
report_globals=0
handle_abort=0
allow_user_poisoning=1
log_exe_name=true
handle_segv=0
detect_stack_use_after_return=0
print_module_map=2
handle_sigbus=0
// [End set_asan]
/**
 * 最佳实践：使用Asan检测内存错误
 * 场景三：方式一-流水线场景
 */

// [Start pipeline_scenario]
hvigorw [taskNames...] ohos-debug-asan=true  <options> 
// [End pipeline_scenario]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景五：方拾二-流水线场景
 */

// [Start pipeline_scenario_two]
hvigorw [taskNames...]  <options> 
// [End pipeline_scenario_two]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景五：heap-buffer-overflow-错误示例
 */

// [Start heap_buffer_overflow]
int heapBufferOverflow() {
    char *buffer;
    buffer = (char *)malloc(10);
    *(buffer + 11) = 'n';
    *(buffer + 12) = 'n';
    free(buffer);
    return buffer[1];
}
// [End heap_buffer_overflow]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景六：heap-buffer-overflow-定位思路
 */

// [Start heap_buffer_overflow_ideas]
Reason:AddressSanitizer:heap-buffer-overflow
Fault thread info:
==appspawn==17140==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x0060019ca8da at pc 0x005ec33c3250 bp 0x007fe9c392f0 sp 0x007fe9c392e8
WRITE of size 1 at 0x0060019ca8da thread T0 (easandemo_api12)
    #0 0x5ec33c324c  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x324c) (BuildId: 4f31be36da7e9bc00c9b7bad563e7ccfec4d0347)
    #1 0x5ec33c38e0  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x38e0) (BuildId: 4f31be36da7e9bc00c9b7bad563e7ccfec4d0347)
    #2 0x7f850b3780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
0x0060019ca8da is located 0 bytes to the right of 10-byte region [0x0060019ca8d0,0x0060019ca8da)
allocated by thread T0 (easandemo_api12) here:
    #0 0x7f82652758  (/system/lib64/libclang_rt.asan.so+0xd2758) (BuildId: aeec20776cc4e8f96db6c6b5603bb49748cc20ff)
    #1 0x5ec33c31ec  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x31ec) (BuildId: 4f31be36da7e9bc00c9b7bad563e7ccfec4d0347)
    #2 0x5ec33c38e0  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x38e0) (BuildId: 4f31be36da7e9bc00c9b7bad563e7ccfec4d0347)
    #3 0x7f850b3780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
    #4 0x5ec6a1bcd8  (/system/lib64/module/arkcompiler/stub.an+0x1dccd8)
    #5 0x5ec6847f4c  (/system/lib64/module/arkcompiler/stub.an+0x8f4c)
// [End heap_buffer_overflow_ideas]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景七：stack-buffer-overflow-错误示例
 */

// [Start stack_buffer_overflow]
int stackBufferOverflow() {
    int subscript = 43;
    char buffer[42];
    buffer[subscript] = 42;
    return 0;
}
// [End stack_buffer_overflow]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景七：stack-buffer-overflow-定位思路
 */

// [Start stack_buffer_overflow_ideas]
Reason:ASAN
=================================================================
==appspawn==8518==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7ffdbb8a1deb at pc 0x7f4d58ec95a6 bp 0x7ffdbb8a1d90 sp 0x7ffdbb8a1d88
WRITE of size 1 at 0x7ffdbb8a1deb thread T0 (e.mycppasandemo)
    #0 0x7f4d58ec95a5  (/data/storage/el1/bundle/libs/x86_64/libentry.so+0x95a5) (BuildId: 5f94771f88ac6f3ed4c63d5c52598c94dc7bca66)
    #1 0x7f4d58eca203  (/data/storage/el1/bundle/libs/x86_64/libentry.so+0xa203) (BuildId: 5f94771f88ac6f3ed4c63d5c52598c94dc7bca66)
    #2 0x7f4dc9c0378f  (/system/lib64/platformsdk/libace_napi.z.so+0x4378f) (BuildId: 88b8b49edb64385b2d6b854950877489)


Address 0x7ffdbb8a1deb is located in stack of thread T0 (e.mycppasandemo) at offset 75 in frame
    #0 0x7f4d58ec948f  (/data/storage/el1/bundle/libs/x86_64/libentry.so+0x948f) (BuildId: 5f94771f88ac6f3ed4c63d5c52598c94dc7bca66)
// [End stack_buffer_overflow_ideas]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景八：stack-buffer-underflow-错误示例
 */

// [Start stack_buffer_underflow]
int stackBufferUnderflow() {
    int subscript = -1;
    char buffer[42];
    buffer[subscript] = 42;
    return 0;
}
// [End stack_buffer_underflow]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景八：stack-buffer-underflow-定位思路
 */

// [Start stack_buffer_underflow_ideas]
Reason:AddressSanitizer:stack-buffer-underflow
Fault thread info:
==appspawn==17039==ERROR: AddressSanitizer: stack-buffer-underflow on address 0x007e07c6027f at pc 0x007f1bdc3994 bp 0x007e07c60250 sp 0x007e07c60248
WRITE of size 1 at 0x007e07c6027f thread T0 (easandemo_api12)
    #0 0x7f1bdc3990  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3990) (BuildId: e34349d8024d23ca83c7c7c3b9f69505d2beb3a0)
    #1 0x7f1bdc3fa8  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3fa8) (BuildId: e34349d8024d23ca83c7c7c3b9f69505d2beb3a0)
    #2 0x7e838339a8  (/system/lib64/platformsdk/libace_napi.z.so+0x339a8) (BuildId: f48b24ee6f099a2107ef30b4ace050de)
Address 0x007e07c6027f is located in stack of thread T0 (easandemo_api12) at offset 31 in frame
    #0 0x7f1bdc3820  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3820) (BuildId: e34349d8024d23ca83c7c7c3b9f69505d2beb3a0)
// [End stack_buffer_underflow_ideas]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景九：heap-use-after-free-错误示例
 */

// [Start heap_use_after_free]
#include <stdlib.h>
int main() {
    int *array = new int[5];
    delete[] array;
    return array[5]; 
}
// [End heap_use_after_free]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景十：heap-use-after-free-定位思路
 */

// [Start heap_use_after_free_ideas]
Reason:AddressSanitizer:heap-use-after-free
Fault thread info:
==appspawn==10126==ERROR: AddressSanitizer: heap-use-after-free on address 0x006121870ce4 at pc 0x005ee1ec321c bp 0x007ff5959310 sp 0x007ff5959308
READ of size 4 at 0x006121870ce4 thread T0 (easandemo_api12)
    #0 0x5ee1ec3218  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3218) (BuildId: 3b906822a911c973ab89188662a589eeedf639a4)
    #1 0x5ee1ec3714  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3714) (BuildId: 3b906822a911c973ab89188662a589eeedf639a4)
    #2 0x7fa9133780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
0x006121870ce4 is located 0 bytes to the right of 20-byte region [0x006121870cd0,0x006121870ce4)
freed by thread T0 (easandemo_api12) here:
    #0 0x7fa569f0c4  (/system/lib64/libclang_rt.asan.so+0xdf0c4) (BuildId: aeec20776cc4e8f96db6c6b5603bb49748cc20ff)
    #1 0x5ee1ec31b8  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x31b8) (BuildId: 3b906822a911c973ab89188662a589eeedf639a4)
    #2 0x5ee1ec3714  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3714) (BuildId: 3b906822a911c973ab89188662a589eeedf639a4)
    #3 0x7fa9133780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
    #4 0x5ee571bcd8  (/system/lib64/module/arkcompiler/stub.an+0x1dccd8)
    #5 0x5ee5547f4c  (/system/lib64/module/arkcompiler/stub.an+0x8f4c)
previously allocated by thread T0 (easandemo_api12) here:
    #0 0x7fa569e888  (/system/lib64/libclang_rt.asan.so+0xde888) (BuildId: aeec20776cc4e8f96db6c6b5603bb49748cc20ff)
    #1 0x5ee1ec3194  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3194) (BuildId: 3b906822a911c973ab89188662a589eeedf639a4)
    #2 0x5ee1ec3714  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3714) (BuildId: 3b906822a911c973ab89188662a589eeedf639a4)
    #3 0x7fa9133780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
    #4 0x5ee571bcd8  (/system/lib64/module/arkcompiler/stub.an+0x1dccd8)
    #5 0x5ee5547f4c  (/system/lib64/module/arkcompiler/stub.an+0x8f4c)
// [End heap_use_after_free_ideas]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景十一：stack-use-after-scope-错误示例
 */

// [Start stack_use_after_scope]
int *gp;
bool b = true;
int stackUseAfterScope() {
    if (b) {
        int x[5];
        gp = x + 1;
    }
    return *gp;
}
// [End stack_use_after_scope]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景十二：stack-use-after-scope-定位思路
 */

// [Start stack_use_after_scope_ideas]
Reason:AddressSanitizer:stack-use-after-scope
Fault thread info:
==appspawn==7494==ERROR: AddressSanitizer: stack-use-after-scope on address 0x007ffa213b44 at pc 0x005ebf0431e4 bp 0x007ffa213b10 sp 0x007ffa213b08
READ of size 4 at 0x007ffa213b44 thread T0 (easandemo_api12)
    #0 0x5ebf0431e0  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x31e0) (BuildId: cf28a04a79da128bc344416e8d5f860e3e22f495)
    #1 0x5ebf0437f4  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x37f4) (BuildId: cf28a04a79da128bc344416e8d5f860e3e22f495)
    #2 0x7f868b3780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
Address 0x007ffa213b44 is located in stack of thread T0 (easandemo_api12) at offset 36 in frame
    #0 0x5ebf043024  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3024) (BuildId: cf28a04a79da128bc344416e8d5f860e3e22f495)
// [End stack_use_after_scope_ideas]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景十三：attempt-free-nonallocated-memory-错误示例
 */

// [Start attempt_free_nonallocated_memory]
int main() {
    int value = 42;
    free(&value);
    return 0;
}
// [End attempt_free_nonallocated_memory]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景十四：attempt-free-nonallocated-memory-定位思路
 */

// [Start attempt_free_nonallocated_memory_ideas]
Reason:AddressSanitizer:attempting
Fault thread info:
==appspawn==20382==ERROR: AddressSanitizer: attempting free on address which was not malloc()-ed: 0x007fd59ae8c0 in thread T0 (easandemo_api12)
    #0 0x7f83a92630  (/system/lib64/libclang_rt.asan.so+0xd2630) (BuildId: aeec20776cc4e8f96db6c6b5603bb49748cc20ff)
    #1 0x5ec45c3120  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3120) (BuildId: 743109db136e66f875a7bc47db74a8095758d4ff)
    #2 0x5ec45c3720  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x3720) (BuildId: 743109db136e66f875a7bc47db74a8095758d4ff)
    #3 0x7f8a2f3780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
Address 0x007fd59ae8c0 is located in stack of thread T0 (easandemo_api12) at offset 32 in frame
    #0 0x5ec45c2fbc  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x2fbc) (BuildId: 743109db136e66f875a7bc47db74a8095758d4ff)
// [End attempt_free_nonallocated_memory_ideas]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景十五：double-free-错误示例
 */

// [Start double_free]
int main() {
    int *x = new int[42];
    delete [] x;
    delete [] x;
    return 0;
}
// [End double_free]

/**
 * 最佳实践：使用Asan检测内存错误
 * 场景十六：double-free-定位思路
 */

// [Start double_free_ideas]
Reason:AddressSanitizer:attempting
Fault thread info:
==appspawn==9596==ERROR: AddressSanitizer: attempting double-free on 0x0061303ecc10 in thread T0 (easandemo_api12):
    #0 0x7fb3292630  (/system/lib64/libclang_rt.asan.so+0xd2630) (BuildId: aeec20776cc4e8f96db6c6b5603bb49748cc20ff)
    #1 0x5ef0b82ef4  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x2ef4) (BuildId: 5b44777ffb29e6665852feeb6f23712aef424077)
    #2 0x5ef0b834bc  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x34bc) (BuildId: 5b44777ffb29e6665852feeb6f23712aef424077)
    #3 0x7fb4af3780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
0x0061303ecc10 is located 0 bytes inside of 32-byte region [0x0061303ecc10,0x0061303ecc30)
freed by thread T0 (easandemo_api12) here:
    #0 0x7fb3292630  (/system/lib64/libclang_rt.asan.so+0xd2630) (BuildId: aeec20776cc4e8f96db6c6b5603bb49748cc20ff)
    #1 0x5ef0b82eec  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x2eec) (BuildId: 5b44777ffb29e6665852feeb6f23712aef424077)
    #2 0x5ef0b834bc  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x34bc) (BuildId: 5b44777ffb29e6665852feeb6f23712aef424077)
    #3 0x7fb4af3780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
    #4 0x5ef459bcd8  (/system/lib64/module/arkcompiler/stub.an+0x1dccd8)
    #5 0x5ef43c7f4c  (/system/lib64/module/arkcompiler/stub.an+0x8f4c)
previously allocated by thread T0 (easandemo_api12) here:
    #0 0x7fb3292758  (/system/lib64/libclang_rt.asan.so+0xd2758) (BuildId: aeec20776cc4e8f96db6c6b5603bb49748cc20ff)
    #1 0x5ef0b82ee0  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x2ee0) (BuildId: 5b44777ffb29e6665852feeb6f23712aef424077)
    #2 0x5ef0b834bc  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x34bc) (BuildId: 5b44777ffb29e6665852feeb6f23712aef424077)
    #3 0x7fb4af3780  (/system/lib64/platformsdk/libace_napi.z.so+0x33780) (BuildId: 25f88248f530c20439061db9eb4ed152)
    #4 0x5ef459bcd8  (/system/lib64/module/arkcompiler/stub.an+0x1dccd8)
    #5 0x5ef43c7f4c  (/system/lib64/module/arkcompiler/stub.an+0x8f4c)
// [End double_free_ideas]
