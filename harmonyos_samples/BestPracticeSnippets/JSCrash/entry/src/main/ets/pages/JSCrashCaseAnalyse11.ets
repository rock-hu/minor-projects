/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start xxx_is_not_initialized_case1]
import { a } from './JSCrashCaseAnalyse10'
export class Animal {
  static {
    console.log("this is in class");
    let str = a; // 报错信息：a is not initialized
  }
}
// [End xxx_is_not_initialized_case1]
