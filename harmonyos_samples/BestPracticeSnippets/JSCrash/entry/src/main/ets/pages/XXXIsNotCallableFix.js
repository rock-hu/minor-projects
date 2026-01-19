/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start JSCrashCaseAnalyse7]
function b(a) {
    a.trim();
}

export function c1() {
    let a = 123;
    b(a.toString()); // 转换为字符串 
}

// [Start JSCrashCaseAnalyse7]