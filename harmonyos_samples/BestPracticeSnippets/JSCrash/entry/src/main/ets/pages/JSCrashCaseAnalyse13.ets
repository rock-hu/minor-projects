/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start xxx_is_not_initialized_fix1]
import { a } from './JSCrashCaseAnalyse10'
export class Animal {
  static {
    console.log("this is in class");
  }
  str = a;  // 修改点
}
// [End xxx_is_not_initialized_fix1]
