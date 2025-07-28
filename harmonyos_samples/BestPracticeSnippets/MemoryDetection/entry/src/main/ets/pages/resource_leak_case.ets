/**
 * 最佳实践：资源泄漏案例
 */

// [Start js_mem_leak_case_1]
/**
 * 设置在每天固定时间固定时刻刷新
 */
function setScheduledTask(hour: number, minute: number, second: number) {
    let taskTime = new Date();
    taskTime.setHours(hour);
    taskTime.setMinutes(minute, second);
    let timeDiff: number = taskTime.getTime() - (new Date()).getTime(); // 获取时间差
    timeDiff = timeDiff > 0 ? timeDiff : (timeDiff + dailyDiff);
    setTimeout(() => {
        this.update(this.dateCallBack); // 经过 timeDiff 后首次执行
        setInterval(() => {
            // 后续一天刷新一次
            this.update(this.dateCallBack);
        }, dailyDiff);
    }, timeDiff);
}
// [End js_mem_leak_case_1]

// [Start thread_leak_case_1]
if (ids.size == 0) {
    if (httpClientBundle.mHttpClient.getRequestSize() == 0) {
        // 需要等所有请求结束后才可以释放
        console.debug('[HttpClientManager.ReleaseHttpClient]Release HttpClientManager.');
        this.mHttpClientMap.delete(httpClientBundle.mHttpClientBundleId);
        httpClientBundle.mHttpClient.releaseHttpClient();
        return;
    }
    console.error("Have uncontrolled IDs exists.");
}
// [End thread_leak_case_1]