/**
 * 最佳实践：JSCrash问题案例分析
 */

// [Start JSCrashCaseAnalyse11]
import { a } from './XXXIsNotInitializedCase'

export class Animal {
  static {
    console.log("this is in class");
    let str = a; // 报错信息：a is not initialized
  }
}

// [End JSCrashCaseAnalyse11]

