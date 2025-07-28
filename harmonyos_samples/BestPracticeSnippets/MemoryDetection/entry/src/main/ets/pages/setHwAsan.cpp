/**
* 最佳实践：使用HWAsan检测内存错误
* 场景二：配置HWASan-流水线场景hvigorw命令
*/

// [Start hw_asan_hvigorw]
hvigorw [taskNames...] -p ohos-enable-hwasan=true  <options>
// [End hw_asan_hvigorw]

/**
* 最佳实践：使用HWAsan检测内存错误
* 场景四：配置HWASan-方式二 流水线场景hvigorw命令
*/

// [Start hw_asan_hvigorw_two]
hvigorw [taskNames...]  <options>
// [End hw_asan_hvigorw_two]

/**
* 最佳实践：使用HWAsan检测内存错误
* 场景五：stack tag-mismatch-错误示例
*/

// [Start stack_tag_mismatch]
// stack-buffer-overflow
int stackBufferOverflow() {
    int subscript = 43;
    char buffer[42];
    buffer[subscript] = 42;
    return 0;
}


// stack-buffer-underflow
int stackBufferUnderflow() {
    int subscript = -1;
    char buffer[42];
    buffer[subscript] = 42;
    return 0;
}


// stack-use-after-return
int *ptr;
__attribute__((noinline))
void FunctionThatEscapesLocalObject() {
  int local[100];
  ptr = &local[0];
}
int main(int argc, char **argv) {
  FunctionThatEscapesLocalObject();
  return ptr[argc];
}
// [End stack_tag_mismatch]

/**
* 最佳实践：使用HWAsan检测内存错误
* 场景六：stack tag-mismatch-定位思路
*/

// [Start stack_tag_mismatch_ideas]
Reason:HWASAN
==appspawn==61390==ERROR: HWAddressSanitizer: tag-mismatch on address 0x007eb11cc05f at pc 0x005acf446438
WRITE of size 1 at 0x007eb11cc05f tags: f1/00 (ptr/mem) in thread T0
    #0 0x5acf446438  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6438) (BuildId: 4b0b8d2189a7eb99fff81c6bc8889dfefd4af4a1)
    #1 0x5acf446c08  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6c08) (BuildId: 4b0b8d2189a7eb99fff81c6bc8889dfefd4af4a1)
    #2 0x5ab397cdc8  (/system/lib64/platformsdk/libace_napi.z.so+0x3cdc8) (BuildId: dc293a22b36a4db17dc689ff9413b64e)


Cause: stack tag-mismatch
Address 0x007eb11cc05f is located in stack of thread T0
Thread: T0 0x005b00002000 stack: [0x007eb09d2000,0x007eb11d1000) sz: 8384512 tls: [0x005aacd0fa98,0x005aacd10279)
Previously allocated frames:
  record_addr:0x5ab053c788 record:0xcc0a005acf4463c0  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x63c0) (BuildId: 4b0b8d2189a7eb99fff81c6bc8889dfefd4af4a1)
  record_addr:0x5ab053c780 record:0xcc1a005acf446a68  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6a68) (BuildId: 4b0b8d2189a7eb99fff81c6bc8889dfefd4af4a1)
  record_addr:0x5ab053c778 record:0xcb81005acf4866ac  (/data/storage/el1/bundle/libs/arm64/libmainpage.so+0x66ac) (BuildId: 10ffa0d04cbc27e12a59f658b5932674185d63ee)
  record_addr:0x5ab053c770 record:0xcc87005acf4465b4  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x65b4) (BuildId: 4b0b8d2189a7eb99fff81c6bc8889dfefd4af4a1)
// [End stack_tag_mismatch_ideas]

/**
* 最佳实践：使用HWAsan检测内存错误
* 场景七：heap-buffer-overflow-错误示例
*/

// [Start hw_heap_buffer_overflow]
// heap-buffer-overflow
void heapBufferOverflow() {
    char *buffer;
    buffer = (char *)malloc(10);
    *(buffer + 11) = 'n';
    *(buffer + 12) = 'n';
    free(buffer);
}

// heap-buffer-underflow
void heapBufferUnderflow() {
    char *buffer;
    buffer = (char *)malloc(10);
    *(buffer - 11) = 'n';
    *(buffer - 12) = 'n';
    free(buffer);
}
// [End hw_heap_buffer_overflow]

/**
* 最佳实践：使用HWAsan检测内存错误
* 场景八：heap-buffer-overflow-定位思路
*/

// [Start hw_heap_buffer_overflow_ideas]
Reason:HWASAN
==appspawn==8344==ERROR: HWAddressSanitizer: tag-mismatch on address 0x000100760332 at pc 0x005adb286570
WRITE of size 1 at 0x000100760332 tags: cb/08(44) (ptr/mem) in thread T0
    #0 0x5adb286570  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6570) (BuildId: 4724eac5a66f4994a03c023a9958da3897de5f16)
    #1 0x5adb286c8c  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6c8c) (BuildId: 4724eac5a66f4994a03c023a9958da3897de5f16)
    #2 0x5abd63cdc8  (/system/lib64/platformsdk/libace_napi.z.so+0x3cdc8) (BuildId: dc293a22b36a4db17dc689ff9413b64e)


[0x000100760320,0x000100760340) is a small allocated heap chunk; size: 32 offset: 18


Cause: heap-buffer-overflow
0x000100760332 is located 110 bytes to the left of 10-byte region [0x0001007603a0,0x0001007603aa)
allocated here:
    #0 0x5a394625ec  (/system/lib64/libclang_rt.hwasan.so+0x225ec) (BuildId: 2b2455ae77181bfdfbfa9561b4e6e3ea376ec93b)
    #1 0x5adb286548  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6548) (BuildId: 4724eac5a66f4994a03c023a9958da3897de5f16)
    #2 0x5adb286c8c  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6c8c) (BuildId: 4724eac5a66f4994a03c023a9958da3897de5f16)
    #3 0x5abd63cdc8  (/system/lib64/platformsdk/libace_napi.z.so+0x3cdc8) (BuildId: dc293a22b36a4db17dc689ff9413b64e)
    #4 0x5ad4373b6c  (/system/lib64/module/arkcompiler/stub.an+0x3f3b6c)
    #5 0x5ad3f8be8c  (/system/lib64/module/arkcompiler/stub.an+0xbe8c)
// [End hw_heap_buffer_overflow_ideas]

/**
* 最佳实践：使用HWAsan检测内存错误
* 场景九：Use-after-free-错误示例
*/

// [Start hw_use_after_free]
// heap-use-after-free
int useAfterFree(int argc) {
    int *array = new int[100];
    delete[] array;
    return array[argc];
}

// double free
void doubleFree() {
    char *p = (char *)malloc(32 * sizeof(char));
    free(p);
    free(p);
}
// [End hw_use_after_free]

/**
* 最佳实践：使用HWAsan检测内存错误
* 场景十：Use-after-free-定位思路
*/

// [Start hw_use_after_free_ideas]
Reason:HWASAN
==appspawn==10741==ERROR: HWAddressSanitizer: tag-mismatch on address 0x000d00036a68 at pc 0x005ab24864c4
READ of size 4 at 0x000d00036a68 tags: ae/6e (ptr/mem) in thread T0
    #0 0x5ab24864c4  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x64c4) (BuildId: e073772c81ab52894a21e4190a263680198f3e9c)
    #1 0x5ab2486c84  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6c84) (BuildId: e073772c81ab52894a21e4190a263680198f3e9c)
    #2 0x5a96b7cdc8  (/system/lib64/platformsdk/libace_napi.z.so+0x3cdc8) (BuildId: dc293a22b36a4db17dc689ff9413b64e)


[0x000d00036a60,0x000d00036c00) is a small unallocated heap chunk; size: 416 offset: 8


Cause: use-after-free
0x000d00036a68 is located 8 bytes inside of 400-byte region [0x000d00036a60,0x000d00036bf0)
freed by thread T0 here:
    #0 0x5a90a6844c  (/system/lib64/libclang_rt.hwasan.so+0x2844c) (BuildId: 2b2455ae77181bfdfbfa9561b4e6e3ea376ec93b)
    #1 0x5ab2486494  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6494) (BuildId: e073772c81ab52894a21e4190a263680198f3e9c)
    #2 0x5ab2486c84  (/data/storage/el1/bundle/libs/arm64/libentry.so+0x6c84) (BuildId: e073772c81ab52894a21e4190a263680198f3e9c)
    #3 0x5a96b7cdc8  (/system/lib64/platformsdk/libace_napi.z.so+0x3cdc8) (BuildId: dc293a22b36a4db17dc689ff9413b64e)
    #4 0x5aab6b3b6c  (/system/lib64/module/arkcompiler/stub.an+0x3f3b6c)
    #5 0x5aab2cbe8c  (/system/lib64/module/arkcompiler/stub.an+0xbe8c)
// [End hw_use_after_free_ideas]