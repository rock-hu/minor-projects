/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start xxx_is_not_callable_fix]
function b(a) {
    a.trim();
}
export function c() {
    let a = 123;    
    b(a.toString()); // 转换为字符串 
}
// [End xxx_is_not_callable_fix]