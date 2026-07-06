/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start JSCrashCaseAnalyse6]
function b(a) {
    a.trim(); // 报错undefined is not callable;
}

export function c() {
    let a = 123;
    b(a);
}

// [End JSCrashCaseAnalyse6]