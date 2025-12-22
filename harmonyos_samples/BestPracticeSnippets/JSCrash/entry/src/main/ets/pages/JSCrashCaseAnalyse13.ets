/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start JSCrashCaseAnalyse13]
import { a } from './JSCrashCaseAnalyse10'
export class Animal {
  static {
    console.log("this is in class");
  }
  str = a;  // 修改点
}
// [End JSCrashCaseAnalyse13]
