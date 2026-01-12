/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start xxx_is_not_callable_case]
function b(a) {
    a.trim(); // 报错undefined is not callable;
}
export function c() {
    let a = 123;
    b(a);
}
// [End xxx_is_not_callable_case]