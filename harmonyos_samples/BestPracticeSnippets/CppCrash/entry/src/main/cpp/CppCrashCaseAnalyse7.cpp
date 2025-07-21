//
// Created on 2025/7/17.
//

/**
 * 最佳实践：CppCrash类问题案例
 */

// [Start trigger_crash_7]
char g_ori[12] = "hello!";

int main()
{
    unsigned long context = 0xa51f;
    void *addr = (void*)(&SupportPac::SupportPacPrint);
    char a[8] = {};
    memcpy(a, &g_ori, sizeof(g_ori)); // 伪造内存溢出攻击，篡改addr内容
    __asm__ __volatile__("autia %0, %1\n\t"
        "blr %0\n\t"
        : "+r"(addr) : "r"(context):);
    print("%s\n", a);
    return 0;
}
// [End trigger_crash_7]