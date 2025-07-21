/**
 * 最佳实践：AppFreeze 类问题优化建议
 */

// [Start appfreeze_advise1]
int xxx() {
    // ...
    mutex_.lock();
    AIContext aiContext;
    if (ContextRpcHandle2AIContext(inputs[0], aiContext) != aicp::SUCCESS) {
        return FAILED;
        // 没有释放锁
    }
    // 没有释放锁
}
// [End appfreeze_advise1]